#include "global.h"

void TableCatalogue::insertTable(Table *table)
{
    logger.log("TableCatalogue::~insertTable");
    this->tables[table->tableName] = table;
}
void TableCatalogue::deleteTable(string tableName)
{
    logger.log("TableCatalogue::deleteTable");
    this->tables[tableName]->unload();
    delete this->tables[tableName];
    this->tables.erase(tableName);
}
bool TableCatalogue::renameMatrix(string &oldTableName, string &newTableName)
{
    logger.log("TableCatalogue::renameMatrix");
    Table *table = this->tables[oldTableName];
    this->tables[newTableName] = table;
    this->tables.erase(oldTableName);
    return true;
}
Table *TableCatalogue::getTable(string tableName)
{
    logger.log("TableCatalogue::getTable");
    Table *table = this->tables[tableName];
    return table;
}
bool TableCatalogue::isTable(string tableName)
{
    logger.log("TableCatalogue::isTable");
    if (this->tables.count(tableName))
        return true;
    return false;
}

bool TableCatalogue::isColumnFromTable(string columnName, string tableName)
{
    logger.log("TableCatalogue::isColumnFromTable");
    if (this->isTable(tableName))
    {
        Table *table = this->getTable(tableName);
        if (table->isColumn(columnName))
            return true;
    }
    return false;
}

void TableCatalogue::print()
{
    logger.log("TableCatalogue::print");
    cout << "\nRELATIONS" << endl;

    int rowCount = 0;
    for (auto rel : this->tables)
    {
        cout << rel.first << endl;
        rowCount++;
    }
    printRowCount(rowCount);
}

TableCatalogue::~TableCatalogue()
{
    logger.log("TableCatalogue::~TableCatalogue");
    for (auto table : this->tables)
    {
        if (table.second->isMatrix)
            table.second->matrixUnload();
        else
            table.second->unload();
        delete table.second;
    }
}
