#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- ORDER BY <attribute> ASC|DESC ON <tableName>
 * 
 */
bool syntacticParseOrderBy(){
    logger.log("syntacticParseOrderBy");
    if(tokenizedQuery.size() != 8 || tokenizedQuery[6] != "ON") {
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = ORDER_BY;
    parsedQuery.orderByResultRelationName = tokenizedQuery[0];
    parsedQuery.orderByRelationName = tokenizedQuery[7];
    parsedQuery.orderByAttr = tokenizedQuery[4];
    parsedQuery.orderByStrategy = tokenizedQuery[5];
    return true;
}

bool semanticParseOrderBY(){
    logger.log("semanticParseOrderBY");

    if(tableCatalogue.isTable(parsedQuery.orderByResultRelationName)){
        cout<<"SEMANTIC ERROR: Result Relation already exist"<<endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.orderByRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.orderByAttr, parsedQuery.orderByRelationName)) {
        cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
        return false;
    }

    return true;
}

void executeOrderBY(){
    // EMPORD <- ORDER BY Super_ssn ASC ON EMPLOYEE
    logger.log("executeOrderBY");
    Table *table = tableCatalogue.getTable(parsedQuery.orderByRelationName);

    vector<string> sortAttributes = {parsedQuery.orderByAttr};
    vector<string> sortAttributesStrategy = {parsedQuery.orderByStrategy};

    table->sortRelation(sortAttributes, sortAttributesStrategy, parsedQuery.orderByResultRelationName);
    return;
}