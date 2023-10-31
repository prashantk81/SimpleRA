## Data System - M'23(Project Phase-2)
## Team - 40
## Commands
##### 1- SORT
-  **Syntax:** `SORT <table_name> BY <column_name1, column_name2,..., column_namek> IN <ASC|DESC, ASC|DESC,..., ASC|DESC>`
-  **Logic:** 
    -  `Sorting and Merging Phase(Pages)`\
    1- Firstly individual pages of the relation are sorted using a custom comparator.\
    2- The comparator takes two rows of a page at a time and checks for the equality of the two attributes in the priority order given at the time of input.\
    3- If the value of any two rows corresponding to the given attributes do not match then we order them based on the provided sorting strategy (ASC|DESC).\
    4- If all the attributes match, then we find the index of the two rows and return the row with the smaller index. This is done to maintain relative ordering among same tuples.\
    5- After each page is sorted, we merge them using a priority_queue.\
    6- Each element of this priority_queue is of the form `<row, <pageIdx, rowIdx>>`.\
    7- row - is an entire row from page[pageIdx][rowIdx]. Here page is the collection of all the pages whose size is = min(number of blocks that corresponding relation was broken into, BUFFER_SIZE - 1).\
    8- pageIdx - It is the index of the page from which the data was picked.\
    9- rowIdx - Represents the index of the row from which the data was picked from page[pageIdx].\
    10- After popping an element from the priority_queue, we write into a temporary table and insert the next row from the same pageIdx the popped element was belonging to.
    - `Merging Phase(Tables)`\
    1- For every table, we maintain a cursor which will be required to fetch its row.\
    2- For merging the sorted pages, we maintain a priority queue using our own custom comparator.\
    3- Each element of the priority queue is of the form `<row, cursorId>`.\
    4- The comparator function sorts the elements based on the sortStrategy (ASC|DESC).\
    5- Once a row is popped out, it is inserted into the resultant table. After that the next row is fetched by the corresponding cursorId and pushed into the priority_queue.\
    `Note -> ` Sort happens `inplace` and the pages of the relation in the temp directory is modified directly. To make the changes propagate to the data directory use `EXPORT`. 
------------------------------------------------------------------------------
##### 2- ORDER BY
-  **Syntax:** `<new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>`
-  **Logic:**\
    1- It is same as sort, only difference being that in ORDER_BY we are only taking into account one attribute on which ordering is to be performed and in SORT there can be multiple attributes.\
    2- So we are just calling the SORT function with the ORDER_BY parameters.
------------------------------------------------------------------------------
##### 3- GROUP BY
-  **Syntax:** `<new_table> <- GROUP BY <grouping_attribute> FROM <table_name> HAVING <aggregate(attribute)> <bin_op> <attribute_value> RETURN <aggregate_func(attribute)>`
-  **Logic:**\
    1- First, we sort the relation using the external sorting algorithm on the basis of given `GROUP_BY` attribute in the ascending order.\
    2- After that we are taking all the tuples corresponding to each grouped value and then apply aggregate function on these tuples given in the `HAVING` clause.\
    3- For all the rows of each group which satisfies the `HAVING` condition, we pass those rows into the aggregate function which is defined in the `RETURN` clause.\
    4- The output is written into the result relation.
------------------------------------------------------------------------------
##### 4- JOIN
-  **Syntax:** `<new_relation_name> <- JOIN <tablename1>, <tablename2> ON <column1> <bin_op> <column2>`
-  **Logic:**\
 1- First we sort both the tables based on join attributes in ascending order.\
 2- Then, we maintain two pointer for both the relations and traverse through the rows.\
 3- Since the rows are now sorted, we know which direction to move as soon as we encounter the first matching value as per the condition.\
 4- Therefore, for one value in the first relation we take all the satisfied values in the second relation based on the given binary operator and then move the first pointer.\
 5- Now a new relation is created with the combined attributes of the first and second relation and write all the satisfied rows in it.
------------------------------------------------------------------------------
## Assumptions
1- Total main memory buffer used = 10 blocks. \
2- In `GROUP_BY`, if aggregate function is `AVG` then we are returning floor value of the actual average to avoid conflicts with `int` and `float` datatypes.
------------------------------------------------------------------------------
## Learnings
1- Sorting the whole relation using a limited set of BLOCKS in external sorting.\
2- Making use of the same comparator to sort the relation involving both types of strategies (ASC|DESC) at the same time.\
3- Joining two different relations based on some condition.\
4- Getting a deeper understanding of the repository while debugging the errors.
------------------------------------------------------------------------------
## Contribution of each member
1 - **Ujjwal Prakash (2022202009)**\
    SORT and ORDER BY \
    `Contribution percentage - 33.33%`\
2 - **Priyank Mahour (2022201047)**\
    JOIN\
    `Contribution percentage - 33.33%`\
3 - **Prashant Kumar(2022201058)**\
    GROUP BY with HAVING clause\
    `Contribution percentage - 33.33%`























