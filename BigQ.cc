#include "BigQ.h"
#include <pthread.h>
#include <unistd.h>
#include "PriorityQueue.h"

struct BigQParams {
    Pipe *in;
    Pipe *out;
    OrderMaker *sortorder;
    Page *currentPage;
    int runlen = 0;
    BigQ *ref;
};

void *proxyFunction(void *args) {
    BigQParams *params;
    params = (BigQParams *) args;
    void *fooPtr = params->ref;
    static_cast<BigQ *>(fooPtr)->Worker(args);
}

BigQ::BigQ(Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
    BigQParams *params = new BigQParams;
    params->in = &in;
    params->out = &out;
    params->sortorder = &sortorder;
    params->runlen = runlen;
    params->ref = this;
    params->currentPage = new Page();

    // thread to dump data into the input pipe (for BigQ's consumption)
    pthread_create(&threadID, NULL, proxyFunction, (void *) params);

}

void *BigQ::Worker(void *args) {
    long numRecs = 0;
    BigQParams *params;
    params = (BigQParams *) args;

    Pipe *in = params->in;
    Pipe *out = params->out;
    OrderMaker *sortorder = params->sortorder;
    int runlen = params->runlen;
    int currPageNum = 0;

    Record *tempRec = new(std::nothrow) Record();
    File *file = new File();
    file->Open(0, "tempSortFile.bin");
    // Page *currentPage = new(std::nothrow) Page();
    Page *currentPage;
    currentPage = params->currentPage;

    while (in->Remove(tempRec) && currPageNum < runlen) {
        if (!currentPage->Append(tempRec)) {
            // sort records before writing to disk
            SortRecords(currentPage, sortorder);
            // write current page to disk
            WritePageToDisk(file, currentPage);
            // increment page number
            currPageNum++;
            // empty the page out
            delete currentPage;
            currentPage = new(std::nothrow) Page();
            // append record to empty page
            currentPage->Append(tempRec);
        }
        numRecs++;
    }

    // deal with the remaining records
    if (currPageNum < runlen) {
        // sort and write records to disk
        SortRecords(currentPage, sortorder);
        WritePageToDisk(file, currentPage);
        // increment page number
        currPageNum++;
        // empty the page out
        delete currentPage;
        // currentPage->EmptyItOut();
        currentPage = new(std::nothrow) Page();
    }

    Page *pages = new Page[runlen];

    // get sorted pages
    for (int i = 0; i < currPageNum; i++) {
        file->GetPage(pages + i, i);
    }

    KWayMerge(pages, out, currPageNum, sortorder);

    delete[] pages;

    // finally shut down the out pipe
    out->ShutDown();
    pthread_exit(NULL);
    return 0;
}

void BigQ::WritePageToDisk(File *file, Page *page) {
    if (!page->pageToDisk) {
        if (!file->GetLength()) {
            file->AddPage(page, file->GetLength());
        } else {
            file->AddPage(page, file->GetLength() - 1);
        }
        // fsync(file->myFilDes);
        page->pageToDisk = true;
    }
}

void BigQ::SortRecords(Page *page, OrderMaker *sortorder) {
    int numRecs = page->numRecs;
    records = new Record[numRecs];

    if (records == NULL) {
        cout << "Error allocating memory for records" << endl;
        exit(1);
    }

    // copy records into the array
    for (int i = 0; i < numRecs; i++) {
        page->GetFirst(records + i);
    }

    // sort the records
    MergeSort(records, 0, numRecs - 1, sortorder);

    // copy the records back into the page
    for (int i = 0; i < numRecs; i++) {
        page->Append(records + i);
    }

    // cleanup
    delete[] records;
}

void BigQ::MergeSort(Record *records, int start, int end, OrderMaker *sortorder) {
    if (start < end) {
        int mid = (start + end) / 2;

        MergeSort(records, start, mid, sortorder);
        MergeSort(records, mid + 1, end, sortorder);
        Merge(records, start, mid, end, sortorder);
    }
}

void BigQ::Merge(Record *records, int start, int mid, int end, OrderMaker *sortorder) {

    ComparisonEngine ceng;

    // create a temp array
    Record *temp = new Record[end - start + 1];

    // crawlers for both intervals and for temp
    int i = start, j = mid + 1, k = 0;

    // traverse both arrays and in each iteration add smaller of both elements in temp
    while (i <= mid && j <= end) {
        if (ceng.Compare(records + i, records + j, sortorder) <= 0) {
            (temp + k)->Consume(records + i);
            k++;
            i++;
        } else {
            (temp + k)->Consume(records + j);
            k++;
            j++;
        }
    }

    // add elements left in the first interval
    while (i <= mid) {
        (temp + k)->Consume(records + i);
        k++;
        i++;
    }

    // add elements left in the second interval
    while (j <= end) {
        (temp + k)->Consume(records + j);
        k++;
        j++;
    }

    // copy temp to original interval
    for (i = start; i <= end; i++) {
        (records + i)->Consume(temp + i - start);
    }

    delete[] temp;
}

void BigQ::KWayMerge(Page *pages, Pipe *out, int runLen, OrderMaker *sortorder) {
    PriorityQueue pq(sortorder, runLen);
    RecordPageNum *tempRec = new RecordPageNum;
    Record *temp = new Record;

    for (int i = 0; i < runLen; i++) {
        (pages + i)->GetFirst(temp);
        tempRec->setRecord(temp);
        tempRec->setPageNumber(i);
        pq.push(tempRec);
    }

    while (!pq.empty()) {
        pq.top(tempRec);
        pq.pop();
        int pageNum = tempRec->getPageNumber();
        out->Insert(tempRec->getRecord());
        if (!(pages + pageNum)->GetFirst(temp)) {
            continue;
        } else {
            tempRec->setRecord(temp);
            tempRec->setPageNumber(pageNum);
            pq.push(tempRec);
        }
    }
}

BigQ::~BigQ() {
}
