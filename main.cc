
#include <iostream>
#include "Record.h"
#include <stdlib.h>
#include "DBFile.h"

using namespace std;

extern "C" {
int yyparse(void);   // defined in y.tab.c
}

extern struct AndList *final;

int main() {
    DBFile dbFile;
    Schema lineitem("catalog", "lineitem");
    char fileName[] = "data/test.tbl";
//    dbFile.Load(lineitem, fileName);
//    dbFile.Close();

    char tempMain[] = "tempMain";
    dbFile.Create(tempMain, heap, NULL);
    FILE *tableFile = fopen(fileName, "r");

    int count = 0;
    Record tempRec;

    while (tempRec.SuckNextRecord(&lineitem, tableFile) == 1) {
//        tempRec.Print(&lineitem);
        dbFile.Add(&tempRec);
        count++;
    }

    dbFile.MoveFirst();
    cout << "Count for ADD: " << count << endl;
    count = 0;
    dbFile.Add(&tempRec);

//
//    while (dbFile.GetNext(&tempRec)) {
//        tempRec.Print(&lineitem);
//        cout << "Inner count" << count++ << endl;
//    }
//    cout << "The count: " << count << endl;
    return 0;

//	// try to parse the CNF
//	cout << "Enter in your CNF: ";
//  	if (yyparse() != 0) {
//		cout << "Can't parse your CNF.\n";
//		exit (1);
//	}
//
//	// suck up the schema from the file
//	Schema lineitem ("catalog", "lineitem");
//
//	// grow the CNF expression from the parse tree
//	CNF myComparison;
//	Record literal;
//	myComparison.GrowFromParseTree (final, &lineitem, literal);
//
//	// print out the comparison to the screen
//	myComparison.Print ();
//
//	// now open up the text file and start procesing it
//        FILE *tableFile = fopen (/home/suraj/Projects/p1/P1/data/lineitem.tbl", "r");
//
//        Record temp;
//        Schema mySchema ("catalog", "lineitem");
//
//	//char *bits = literal.GetBits ();
//	//cout << " numbytes in rec " << ((int *) bits)[0] << endl;
//	//literal.Print (&supplier);
//
//        // read in all of the records from the text file and see if they match
//	// the CNF expression that was typed in
//	int counter = 0;
//	ComparisonEngine comp;
//        while (temp.SuckNextRecord (&mySchema, tableFile) == 1) {
//		counter++;
//		if (counter % 10000 == 0) {
//			cerr << counter << "\n";
//		}
//
//		if (comp.Compare (&temp, &literal, &myComparison))
//                	temp.Print (&mySchema);
//
//        }

}

//int main() {
//    DBFile dbFile;
//    int count = 0;
//    Schema lineitem("catalog", "lineitem");
//    const char tempMain[] = "tempMain";
//    dbFile.Create(tempMain, heap, NULL);
//    FILE *tableFile = fopen("data/lineitem.tbl", "r");
//    Record tempRec;
//    while (tempRec.SuckNextRecord(&lineitem, tableFile) == 1) {
//        dbFile.Add(&tempRec);
//    }
//    dbFile.Close();
//    dbFile.Load(lineitem, tempMain);
//    while (dbFile.GetNext(&tempRec)) {
//        count++;
//    }
//    cout << "The count: " << count << endl;
//    return 0;
//}




