#include "global.h"
/**
 * @brief
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool syntacticParseJOIN()
{
    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[5] != "ON")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[3];
    parsedQuery.joinSecondRelationName = tokenizedQuery[4];
    parsedQuery.joinFirstColumnName = tokenizedQuery[6];
    parsedQuery.joinSecondColumnName = tokenizedQuery[8];

    string binaryOperator = tokenizedQuery[7];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

bool checkJoinCondition(vector<int> &table1, vector<int> &table2, int t1idx, int t2idx)
{
    int operationValue = parsedQuery.joinBinaryOperator;

    if (operationValue == 0)
        return table1[t1idx] < table2[t2idx];
    else if (operationValue == 1)
        return table1[t1idx] > table2[t2idx];
    else if (operationValue == 2)
        return table1[t1idx] <= table2[t2idx];
    else if (operationValue == 3)
        return table1[t1idx] >= table2[t2idx];
    else if (operationValue == 4)
        return table1[t1idx] == table2[t2idx];
    else if (operationValue == 5)
        return table1[t1idx] != table2[t2idx];
    return false;
}

void executeJOIN()
{
    logger.log("executeJOIN");
    Table *table1 = tableCatalogue.getTable(parsedQuery.joinFirstRelationName);
    Table *table2 = tableCatalogue.getTable(parsedQuery.joinSecondRelationName);

    vector<string> firstCols = table1->columns;
    firstCols.insert(firstCols.end(), table2->columns.begin(), table2->columns.end());

    Table *resultTable = new Table(parsedQuery.joinResultRelationName, firstCols);
    tableCatalogue.insertTable(resultTable);

    vector<string> firstAttr = {parsedQuery.joinFirstColumnName};
    vector<string> secondAttr = {parsedQuery.joinSecondColumnName};
    vector<string> firstSort = {"ASC"};
    vector<string> secondSort = {"ASC"};
    table1->sortRelation(firstAttr, firstSort, "");
    table2->sortRelation(secondAttr, secondSort, "");

    Cursor c1Table1(parsedQuery.joinFirstRelationName, 0);
    Cursor c1Table2(parsedQuery.joinSecondRelationName, 0);
    Cursor c2Table2(parsedQuery.joinSecondRelationName, 0);
    int table1Idx = table1->getColumnIndex(parsedQuery.joinFirstColumnName);
    int table2Idx = table2->getColumnIndex(parsedQuery.joinSecondColumnName);


    vector<int> tempResult;
    for (int i = 0; i < table1->rowCount; i++)
    {
        vector<int> rowOfTable1 = c1Table1.getNext();
        c2Table2 = c1Table2;
        while (true)
        {
            vector<int> temp = c2Table2.getNext();
            if (temp.size() == 0)
                break;
            if (checkJoinCondition(rowOfTable1, temp, table1Idx, table2Idx))
            {
                tempResult = rowOfTable1;
                tempResult.insert(tempResult.end(), temp.begin(), temp.end());
                resultTable->writeRow<int>({tempResult});
                tempResult.clear();
            }
        }
    }
    resultTable->blockify();

    return;
}
