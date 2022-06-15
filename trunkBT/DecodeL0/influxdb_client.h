//----------------------------------------------------------
//  File: influxdb_client.h
//
//  Header file for AMS-02 AMI InfluxDB client
//
//  History:
//    20190730 - created by X.Cai
//----------------------------------------------------------

#include <pthread.h>
#include <curl/curl.h>

#ifndef __INFLUXDB_CLIENT_H
#define __INFLUXDB_CLIENT_H

//----------------------------------------------------------
// Defines
//----------------------------------------------------------

#define DB_BUFF_SIZE    100000

#define DB_DEBUG        1
#define NO_DEBUG        0

//----------------------------------------------------------
// Types
//----------------------------------------------------------

typedef struct __db_client {
    char           *server;         // Server name
    int             port;           // Port number
    char           *user;           // User name
    char           *passwd;         // Password
    char           *db;             // DB name
    char            url[1000];      // URL string
    int             ssl;            // Using SSL
    int             size;           // Data string size
    char           *data;           // Data string pointer
    pthread_mutex_t mutex;          // Mutex for protection
    CURL           *curl;           // handle
} db_client_t;

//----------------------------------------------------------
// Global variables
//----------------------------------------------------------

extern db_client_t *client;
extern int db_test_only;
extern int debug_output;

extern int file_no;

//----------------------------------------------------------
// Functions
//----------------------------------------------------------

extern db_client_t *db_client_init(char *server,        // Server name
                                   int   port,          // port number
                                   char *user,          // User name
                                   char *passwd,        // Password
                                   char *db,            // Db name
                                   int   ssl);          // Using SSL

extern void db_client_data(db_client_t *client,         // DB client descriptor
                           const char *name,            // Signal name (measurement and tags)
                           const char *fields,          // Data fields
                           const uint64_t utime,       // Time in microseconds
                           const int debug);            // Debug flag

extern int db_client_write(db_client_t *client);

#endif  // __INFLUXDB_CLIENT_H
