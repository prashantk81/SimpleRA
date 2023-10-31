#include "global.h"
/**
 * @brief File contains method to process SORT commands.
 *
 * syntax:
 *  T1 <- GROUP BY dno FROM emp HAVING SUM(sal) > 1000 RETURN avg(sal)
 *
 */
bool syntacticParseGROUP()
{
    logger.log("syntacticParseGroup");
    if (tokenizedQuery.size() != 13 || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "HAVING" || tokenizedQuery[11] != "RETURN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = GROUP;
    parsedQuery.groupByResultRelationName = tokenizedQuery[0];
    parsedQuery.groupByGroupAttributeName = tokenizedQuery[4];
    parsedQuery.groupByRelationName = tokenizedQuery[6];

    string havingStrategy = tokenizedQuery[8];
    string havingAggregateFxn = havingStrategy.substr(0, 3);
    if (havingAggregateFxn != "SUM" && havingAggregateFxn != "AVG" && havingAggregateFxn != "MAX" && havingAggregateFxn != "MIN" && havingAggregateFxn != "COU")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if (havingAggregateFxn == "COU")
    {
        if (havingStrategy[3] == 'N' && havingStrategy[4] == 'T')
            parsedQuery.groupByHavingAggregateFunctionName = "COUNT";
        else
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
    else
        parsedQuery.groupByHavingAggregateFunctionName = havingAggregateFxn;

    int len = havingStrategy.size();
    if (parsedQuery.groupByHavingAggregateFunctionName == "COUNT")
    {
        if (havingStrategy[5] != '(' || havingStrategy[len - 1] != ')')
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
    else
    {
        if (havingStrategy[3] != '(' || havingStrategy[len - 1] != ')')
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }

    if (parsedQuery.groupByHavingAggregateFunctionName == "COUNT")
        parsedQuery.groupByHavingAttributeName = havingStrategy.substr(6, len - 7);
    else
        parsedQuery.groupByHavingAttributeName = havingStrategy.substr(4, len - 5);

    string binaryOperator = tokenizedQuery[9];
    if (binaryOperator == "<")
        parsedQuery.groupByBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.groupByBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.groupByBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.groupByBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.groupByBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.groupByBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.groupByHavingConditionValue = tokenizedQuery[10];

    string resultStrategy = tokenizedQuery[12];
    string resultAggregateFxn = resultStrategy.substr(0, 3);
    if (resultAggregateFxn != "SUM" && resultAggregateFxn != "AVG" && resultAggregateFxn != "MAX" && resultAggregateFxn != "MIN" && resultAggregateFxn != "COU")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if (resultAggregateFxn == "COU")
    {
        if (resultStrategy[3] == 'N' && resultStrategy[4] == 'T')
            parsedQuery.groupByReturnaggregateFunctionName = "COUNT";
        else
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
    else
        parsedQuery.groupByReturnaggregateFunctionName = resultAggregateFxn;

    len = resultStrategy.size();
    if (parsedQuery.groupByReturnaggregateFunctionName == "COUNT")
    {
        if (resultStrategy[5] != '(' || resultStrategy[len - 1] != ')')
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
    else
    {
        if (resultStrategy[3] != '(' || resultStrategy[len - 1] != ')')
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }

    if (parsedQuery.groupByReturnaggregateFunctionName == "COUNT")
        parsedQuery.groupByReturnAttributeName = resultStrategy.substr(6, len - 7);
    else
        parsedQuery.groupByReturnAttributeName = resultStrategy.substr(4, len - 5);

    return true;
}

bool semanticParseGROUP()
{
    logger.log("semanticParseGROUP");
    if (tableCatalogue.isTable(parsedQuery.groupByResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.groupByRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupByGroupAttributeName, parsedQuery.groupByRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.groupByHavingAttributeName, parsedQuery.groupByRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.groupByReturnAttributeName, parsedQuery.groupByRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

int solveAggregateFxnOnAllRows(int colIdx, vector<vector<int>> &allRows, string &fxn)
{
    int result;
    int sum = 0;
    int maxi = INT_MIN;
    int mini = INT_MAX;
    int count = allRows.size();
    for (int i = 0; i < count; i++)
    {
        sum += allRows[i][colIdx];
        maxi = max(maxi, allRows[i][colIdx]);
        mini = min(mini, allRows[i][colIdx]);
    }
    if (fxn == "SUM")
        return sum;
    else if (fxn == "MAX")
        return maxi;
    else if (fxn == "MIN")
        return mini;
    else if (fxn == "AVG")
        return sum / count;
    return count;
}

int calculateClauseValue(string &havingAttributeAggregateName, int havingAttributeColumnIdx, vector<vector<int>> &groupByAllRows)
{
    int value;
    if (havingAttributeAggregateName == "MAX")
        value = solveAggregateFxnOnAllRows(havingAttributeColumnIdx, groupByAllRows, havingAttributeAggregateName);
    else if (havingAttributeAggregateName == "MIN")
        value = solveAggregateFxnOnAllRows(havingAttributeColumnIdx, groupByAllRows, havingAttributeAggregateName);
    else if (havingAttributeAggregateName == "AVG")
        value = solveAggregateFxnOnAllRows(havingAttributeColumnIdx, groupByAllRows, havingAttributeAggregateName);
    else if (havingAttributeAggregateName == "SUM")
        value = solveAggregateFxnOnAllRows(havingAttributeColumnIdx, groupByAllRows, havingAttributeAggregateName);
    else
        value = solveAggregateFxnOnAllRows(havingAttributeColumnIdx, groupByAllRows, havingAttributeAggregateName);

    return value;
}

void executeGroupBy()
{
    logger.log("executeSORT");

    Table *table = tableCatalogue.getTable(parsedQuery.groupByRelationName);
    vector<string> sortAttribute = {parsedQuery.groupByGroupAttributeName};
    vector<string> sortStrategy = {"ASC"};
    table->sortRelation(sortAttribute, sortStrategy, "");
    Cursor cursor(parsedQuery.groupByRelationName, 0);
    int groupByColumnIdx = table->getColumnIndex(parsedQuery.groupByGroupAttributeName);
    vector<vector<int>> finalResult;
    vector<vector<int>> groupByAllRows;
    vector<int> firstRowOfGroup;
    string aggCol = parsedQuery.groupByReturnaggregateFunctionName + parsedQuery.groupByReturnAttributeName;
    vector<string> columnList = {parsedQuery.groupByGroupAttributeName, aggCol};
    Table *resultantTable = new Table(parsedQuery.groupByResultRelationName, columnList);
    tableCatalogue.insertTable(resultantTable);
    for (int i = 0; i < table->rowCount; i++)
    {
        // fetching all rows based on group by column
        if (i == 0)
            firstRowOfGroup = cursor.getNext();
        groupByAllRows.push_back(firstRowOfGroup);
        vector<int> temp;
        while (true)
        {
            temp = cursor.getNext();
            if (i + 1 >= table->rowCount || temp[groupByColumnIdx] != firstRowOfGroup[groupByColumnIdx])
                break;
            groupByAllRows.push_back(temp);
            i++;
        }
        // perform having condition
        int havingAttributeColumnIdx = table->getColumnIndex(parsedQuery.groupByHavingAttributeName);
        string havingAttributeAggregateName = parsedQuery.groupByHavingAggregateFunctionName;
        int value = calculateClauseValue(havingAttributeAggregateName, havingAttributeColumnIdx, groupByAllRows);

        int conditionCheckingValue = stoi(parsedQuery.groupByHavingConditionValue);
        int returnAttributeColumnIdx = table->getColumnIndex(parsedQuery.groupByReturnAttributeName);
        string returnAttributeAggregateName = parsedQuery.groupByReturnaggregateFunctionName;
        int resultValue = calculateClauseValue(returnAttributeAggregateName, returnAttributeColumnIdx, groupByAllRows);

        int operationValue = parsedQuery.groupByBinaryOperator;
        if (operationValue == 0 && value < conditionCheckingValue)
            resultantTable->writeRow<int>({{firstRowOfGroup[groupByColumnIdx], resultValue}});
        else if (operationValue == 1 && value > conditionCheckingValue)
            resultantTable->writeRow<int>({{firstRowOfGroup[groupByColumnIdx], resultValue}});
        else if (operationValue == 2 && value <= conditionCheckingValue)
            resultantTable->writeRow<int>({{firstRowOfGroup[groupByColumnIdx], resultValue}});
        else if (operationValue == 3 && value >= conditionCheckingValue)
            resultantTable->writeRow<int>({{firstRowOfGroup[groupByColumnIdx], resultValue}});
        else if (operationValue == 4 && value == conditionCheckingValue)
            resultantTable->writeRow<int>({{firstRowOfGroup[groupByColumnIdx], resultValue}});
        else if (operationValue == 5 && value != conditionCheckingValue)
            resultantTable->writeRow<int>({{firstRowOfGroup[groupByColumnIdx], resultValue}});

        groupByAllRows.clear();
        firstRowOfGroup.clear();
        firstRowOfGroup = temp;
    }
    resultantTable->blockify();
    return;
}