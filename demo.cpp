/* *
* Basic example demonstrating how to to use lcmysql
*
*/

/* Standard C++ includes */
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <memory>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/warning.h>
#include <cppconn/metadata.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include <cppconn/datatype.h>
#include <mysql_driver.h>
#include <mysql_connection.h>

#include <lcmysql/sqlbuilder.h>
#include <lcmysql/sqlquery.h>

using namespace lingcreative::mysql;
using namespace std;

struct Label
{
    int id;
    string value;
    Label(int id, string value){ this->id = id; this->value = value; }
};
class TestMapper : public SqlMapper<Label*>
{
public:
    virtual Label* map(sql::ResultSet& rs)
    {
        int id = rs.getInt("id");
        string value = rs.getString("label").asStdString();
        return new Label(id, value);
    }
};

#if __STDC_VERSION__ < 199901L
#  if __GNUC__ >= 2
#    define __CURRENT_FUNC__ __FUNCTION__
#  else
#    define __CURRENT_FUNC__ "(function n/a)"
#  endif
#elif defined(_MSC_VER)
#  if _MSC_VER < 1300
#    define __CURRENT_FUNC__ "(function n/a)"
#  else
#    define __CURRENT_FUNC__ __FUNCTION__
#  endif
#elif (defined __func__)
#  define __CURRENT_FUNC__ __func__
#else
#  define __CURRENT_FUNC__ "(function n/a)"
#endif

#ifndef __LINE__
  #define __LINE__ "(line number n/a)"
#endif


// Connection properties
#define DEMO_DB   "test"
#define DEMO_HOST "tcp://127.0.0.1:3306"
#define DEMO_USER "root"
#define DEMO_PASS "mariadb"

// Sample data
#define DEMO_NUM_TEST_ROWS 6
struct _test_data {
	int id;
	const char* label;
};
static _test_data test_data[DEMO_NUM_TEST_ROWS] = {
	{1, ""}, {2, "a"}, {3, "b"}, {4, "c"}, {10, "Hello"}, {33, "Killo"}
};


int main(int argc, const char **argv)
{
	static const string url(argc >= 2 ? argv[1] : DEMO_HOST);
	static const string user(argc >= 3 ? argv[2] : DEMO_USER);
	static const string pass(argc >= 4 ? argv[3] : DEMO_PASS);
	static const string database(argc >= 5 ? argv[4] : DEMO_DB);

	/* Driver Manager */
	sql::Driver *driver;

	stringstream sql;
	int i;
	struct _test_data min, max;

	cout << boolalpha;
	cout << "# Connector/C++ result set.." << endl;

	try {
		/* Using the Driver to create a connection */
		driver = sql::mysql::get_driver_instance();
		std::unique_ptr< sql::Connection > con(driver->connect(url, user, pass));

		con->setSchema(database);

		std::unique_ptr< sql::Statement > stmt(con->createStatement());
		stmt->execute("DROP TABLE IF EXISTS test");
		stmt->execute("CREATE TABLE test(id INT, label CHAR(10))");
		cout << "#\t Test table created" << endl;

		/* Populate the test table with data */
		min = max = test_data[0];
		for (i = 0; i < DEMO_NUM_TEST_ROWS; i++) {
			/* Remember mnin/max values for further testing */
			if (test_data[i].id < min.id) {
				min = test_data[i];
			}
			if (test_data[i].id > max.id) {
				max = test_data[i];
			}

			/*
			KLUDGE: You should take measures against SQL injections!
			*/
			sql.str("");
			sql << "INSERT INTO test(id, label) VALUES (";
			sql << test_data[i].id << ", '" << test_data[i].label << "')";
			stmt->execute(sql.str());
		}
		cout << "#\t Test table populated" << endl;


		cout << "========================" << endl;
        SqlBuilder builder;
        builder += "select id, label from test where id > ? and label like ?";
        builder << 1 << "%il%";
        TestMapper mapper;
        shared_ptr< list<Label*> > result = Query<Label*>(*con, builder, mapper);
        cout << "result = list(" << result->size() << ")" << endl;
        for(std::list<Label*>::iterator i = result->begin(); i != result->end(); i++)
        {
            Label& data = **i;
            std::string s = data.value;
            cout << data.id << " = " << s << endl;
        }
        cout << "========================" << endl;

        builder = SqlBuilder();
        builder += "select * from test where id = ?";
        builder << 33;
        shared_ptr<Label> label( QueryOne<Label*>(*con, builder, mapper) );
        cout << label->id << " = " << label->value << endl;
        cout << "========================" << endl;
	} catch (sql::SQLException &e) {
		/*
		The MySQL Connector/C++ throws three different exceptions:

		- sql::MethodNotImplementedException (derived from sql::SQLException)
		- sql::InvalidArgumentException (derived from sql::SQLException)
		- sql::SQLException (derived from std::runtime_error)
		*/
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __CURRENT_FUNC__ << ") on line " << __LINE__ << endl;
		// Use what(), getErrorCode() and getSQLState()
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		cout << "not ok 1 - demo.cpp" << endl;

		return EXIT_FAILURE;
	} catch (std::runtime_error &e) {

		cout << "# ERR: runtime_error in " << __FILE__;
		cout << "(" << __CURRENT_FUNC__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what() << endl;
		cout << "not ok 1 - demo.cpp" << endl;

		return EXIT_FAILURE;
	}

	cout << "ok 1 - demo.cpp" << endl;
	return EXIT_SUCCESS;
}
