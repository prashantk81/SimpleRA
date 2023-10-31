#include "global.h"
/**
 * @brief
 * SYNTAX: CHECKSYMMETRY matrix_name
 */
bool syntacticParseCHECKSYMMETRY()
{
    logger.log("syntacticParseCHECKSYMMETRY");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.symmetryRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCHECKSYMMETRY()
{
    logger.log("semanticParseCHECKSYMMETRY");

    if (!tableCatalogue.isTable(parsedQuery.symmetryRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCHECKSYMMETRY()
{
    logger.log("executeCHECKSYMMETRY");
    Table *table = tableCatalogue.getTable(parsedQuery.symmetryRelationName);
    table->setReadWriteZero();
    if (table->checkSymmetry())
        cout << "TRUE";
    else
        cout << "FALSE";
    cout << endl;
    table->getBlockStatistics();
    return;
}
