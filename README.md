012408-1606P

--updated Lexer.l

# README

### Test driver for Assignment 1 heap DBFile (spring 2019 dbi) :

This test driver gives a menu-based interface to three options that allows you to test your code:

1. load (read a tpch file and write it out a heap DBFile)

2. scan (read records from an existing heap DBFile)

3. scan & filter (read records and filter using a CNF predicate)


5.  add records to file (new test case added)
6. add records and scan (new test added)

Note that the driver only works with the tpch files (generated using the dbgen program).

To compile the driver, type

```make test.out```

To run the driver, type

```test.out```

and then follow the on-screen instructions.

### Using the driver:

1. SETTINGS: The following variables control the various file locations and they are declared in test.cc (just after the #include header declarations):
**o dbfile_dir** -- this is where the created heap db-files will be stored. By default, this is set to "" (thus all the heap dbfiles will be created in the current directory).
**o tpch_dir** -- this stores the directory path where the tpch-files can be found. The "/cise/tmp/dbi_sp11/DATA/" directory already has the required table files generated by TA and the path should be accessible by your program. There are two tpch-files: 10M and 1G. You can select any one to test. By default, tpch_dir is set to "/cise/tmp/dbi_sp11/DATA/10M". If you are running this driver from CISE linux machine, you donot have to change this setting. 
**o catalog_path** -- this stores the catalog file path. By default this is set to "". Again, if you are running the driver from CISE linux machines you donot have to change this setting.

2. Next replace the stub files DBFile.h and DBFile.cc with your own code. If you are using additional headers, modify the Makefile to include them as well. Then compile and execute the driver. Select the load option and convert the tpch-files into heap dbfiles. By default, the heap file is written in the same directory as the executable and have the extension ".bin".

3. Once load of a file has been selected, you can select option 2 or 3 to scan/filter all the records from the heap DBfile.  If option 3 is selected, a CNF should be supplied. Some example CNF's are given below. They are numbered q1,q2..q12. Use the table below to identify the tpch file associated with each CNF.
| table      | CNF     |
| -----------|:-------:|
| region     | q1 q2   |
| nation     | q3      |
| supplier   | q4 q5   |
| customer   | q6      |
| part       | q7      |
| partsupp   | q8 q9   |
| orders     | q10     |
| lineitem   | q11 q12 |


The expected output for these CNF's can be found in the file "output.log"
#### The outputs generated have been included in ```output``` directory 

### Example CNF's


#### q1

```(r_name = 'EUROPE')```

#### q2

```
(r_name &lt; 'MIDDLE EAST') AND (r_regionkey &gt; 1)
```

#### q3
```
(n_regionkey = 3) AND
(n_nationkey &gt; 10) AND
(n_name &gt; 'JAPAN')
```

#### q4
```
(s_suppkey &lt; 10)
```
#### q5
```
(s_nationkey = 18) AND
(s_acctbal &gt; 1000.0) AND
(s_suppkey &lt; 400)
```
#### q6
```
(c_nationkey = 23) AND
(c_mktsegment = 'FURNITURE') AND
(c_acctbal &gt; 7023.99) AND
(c_acctbal &lt; 7110.83)
```
#### q7
```
(p_brand = 'Brand#13') AND
(p_retailprice &gt; 500.0) AND
(p_retailprice &lt; 930.0) AND
(p_size &gt; 28) AND
(p_size &lt; 1000000)
```
#### q8
```
(ps_supplycost &gt; 999.98)
```
#### q9
```
(ps_availqty &lt; 10)
(ps_supplycost &gt; 100.0) AND
(ps_suppkey &lt; 300) AND
```
#### q10
```
(o_orderpriority = '1-URGENT') AND
(o_orderstatus = 'O') AND
(o_shippriority = 0) AND
(o_totalprice &gt; 1015.68) AND
(o_totalprice &lt; 1051.89)
```
#### q11
```
(l_shipdate &gt; '1994-01-01') AND
(l_shipdate &lt; '1994-01-04') AND
(l_discount &gt; 0.04) AND
(l_discount &lt; 0.07) AND
(l_quantity = 4.0)
```
#### q12
```
(l_orderkey &gt; 100) AND
(l_orderkey &lt; 1000) AND
(l_partkey &gt; 100) AND
(l_partkey &lt; 5000) AND
(l_shipmode = 'AIR') AND
(l_linestatus = 'F') AND
(l_tax &lt; 0.07)
```