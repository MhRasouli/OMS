#include "Group_dbt.h"

Group_dbt::Group_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "`group`")
{
}
bool Group_dbt::Select(map<int, Group> &Groups, int Group_id)
{
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "group_id, name", "group_id=%i", Group_id))
        {
            if (table_data.size() > 0)
            {
                Group result;
                result.group_id = atoi(table_data[0][0].c_str());
                result.name = table_data[0][1];

                if (Groups.find(result.group_id) == Groups.end())
                {
                    Groups[result.group_id] = result;
                }
                else
                {
                    Groups[result.group_id].name = result.name;
                }

                res = true;
            }
            else
            {
                Logger::logger.log_special(LogLocation, false, "Id %i does not exist in table %s", Group_id, tableName.c_str());
            }
        }
        else
        {
            Logger::logger.log_special(LogLocation, true, "select from table %s failed", tableName.c_str());
        }
    }
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
	}
	catch (const std::exception &ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		Logger::logger.log_special(LogLocation, true, "Error occurred: %s", ex.what());
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		Logger::logger.log_special(LogLocation, true, "Unknown failure occurred. Possible memory corruption");
	}

    return res;
}
bool Group_dbt::SelectAll(map<int, Group> &Groups)
{
    Groups.clear();
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "group_id, name"))
        {
            if (table_data.size() > 0)
            {
                for (int i = 0; i < table_data.size(); i++)
                {
                    Group result;
                    result.group_id = atoi(table_data[i][0].c_str());
                    result.name = table_data[i][1];

                    Groups[result.group_id]=result;
                }
                res = true;
            }
            else
            {
                Logger::logger.log_special(LogLocation, false, "Table %s is empty", tableName.c_str());
            }
        }
        else
        {
            Logger::logger.log_special(LogLocation, true, "select from table %s failed", tableName.c_str());
        }
    }
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
	}
	catch (const std::exception &ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		Logger::logger.log_special(LogLocation, true, "Error occurred: %s", ex.what());
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		Logger::logger.log_special(LogLocation, true, "Unknown failure occurred. Possible memory corruption");
	}

    return res;
}