#include "database.h"

Database db;

/**
 * @brief function to handle incomming serial port communication
 */
void parseAndStoreData(String data, int pos)
{
  int firstDelimiter = data.indexOf(';');
  int secondDelimiter = data.lastIndexOf(';');

  // Basic check to ensure we have at least two delimiters
  if (firstDelimiter == -1 || secondDelimiter == -1 ||
      firstDelimiter == secondDelimiter)
  {
    Serial.println("Error: Invalid data format.");
    return;
  }

  // Decrypt data
  /*
  const String decryptedID = decrypt_data(data.substring(0, firstDelimiter));
  const String decryptedUsername =
      decrypt_data(data.substring(firstDelimiter + 1, secondDelimiter));
  const String decryptedPassword =
      decrypt_data(data.substring(secondDelimiter + 1));
  */

  // Not decrypted
  const String decryptedID = data.substring(0, firstDelimiter);
  const String decryptedUsername = data.substring(firstDelimiter + 1, secondDelimiter);
  const String decryptedPassword = data.substring(secondDelimiter + 1);

  // Extract ID
  db.id[pos] = decryptedID;
  // Extract Username
  db.username[pos] = decryptedUsername;
  // Extract Password
  db.password[pos] = decryptedPassword;
}

void readSerialData(String receivedData, bool status, int pos)
{
  while (!status)
  {
    if (Serial.available())
    {
      char inChar = (char)Serial.read();
      receivedData += inChar;
      if (inChar == '\n')
      {
        status = true;
      }
    }
  }
  receivedData.trim(); // Remove any trailing newline or spaces
  parseAndStoreData(receivedData, pos);
  receivedData = "";
  status = false;
}

/* SPIFFS DATABASE */
/**
 * @brief initialization function of SPIFFS
 */
void init_spiffs_db(void)
{
  esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 2,
      .format_if_mount_failed = true};
  esp_err_t ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK)
  {
    Serial.println("SPIFFS init failed!");
    return;
  }

  Serial.println("SPIFFS init OK");

  load_db_from_file();
}

/**
 * @brief add entry to a database
 *
 * @param id entry id
 * @param username entry username
 * @param password entry password
 */
void db_add(int id, const char *username, const char *password)
{
  FILE *f = fopen("/spiffs/db.txt", "a");
  if (!f)
    return;

  fprintf(f, "%d;%s;%s\n", id, username, password);
  Serial.printf("%d;%s;%s\n", id, username, password);
  fclose(f);
}

/**
 * @brief Load database from SPIFFS file, this function is part of SPIFFS init function, reads database content at start
 */
void load_db_from_file(void)
{
  FILE *f = fopen("/spiffs/db.txt", "r");
  if (!f)
    return;

  char line[128];
  int i = 0;

  while (fgets(line, sizeof(line), f) && i < DB_LENGTH)
  {
    int id;
    char username[64];
    char password[64];

    if (sscanf(line, "%d;%63[^;];%63[^\n]", &id, username, password) == 3)
    {
      Serial.printf("Read line: %s", line);
      db.id[i] = id;
      db.username[i] = username;
      db.password[i] = password;
      i++;
    }
  }

  fclose(f);
}

/**
 * @brief This function updates database when called
 *
 * @param target_id desired id to be changed
 * @param newusername updated username
 * @param newpassword updated password
 */
void db_update(int target_id, const char *newusername, const char *newpassword)
{
  FILE *old = fopen("/spiffs/db.txt", "r");
  FILE *newf = fopen("/spiffs/tmp.txt", "w");

  if (!newf)
  {
    if (old)
      fclose(old);
    return;
  }

  bool found = false;
  char line[128];

  if (old)
  {
    while (fgets(line, sizeof(line), old))
    {
      int id;
      char user[64];
      char pass[64];

      if (sscanf(line, "%d;%63[^;];%63[^\n]", &id, user, pass) == 3)
      {
        if (id == target_id)
        {
          fprintf(newf, "%d;%s;%s\n", id, newusername, newpassword);
          found = true;
        }
        else
        {
          fprintf(newf, "%d;%s;%s\n", id, user, pass);
        }
      }
      else
      {
        // Write unrecognized lines as-is
        fputs(line, newf);
      }
    }
    fclose(old);
  }

  // If entry was not found, add it as a new entry
  if (!found)
  {
    fprintf(newf, "%d;%s;%s\n", target_id, newusername, newpassword);
  }

  fclose(newf);

  remove("/spiffs/db.txt");
  rename("/spiffs/tmp.txt", "/spiffs/db.txt");
  rename("/spiffs/tmp.txt", "/spiffs/db.txt");
}

/**
 * @brief Saves the in-memory database to a file on the SPIFFS filesystem.
 *
 * Deletes the existing database file ("/spiffs/db.txt") and creates a new one,
 * writing all non-empty database entries in a semicolon-delimited format
 * (id;username;password) with one entry per line.
 *
 * @note Prints an error message to Serial if the file cannot be created.
 *       Prints a confirmation message to Serial upon successful save.
 */
void save_spiffs_db()
{
  // Delete old file
  remove("/spiffs/db.txt");

  // Create new file and write all database entries
  FILE *f = fopen("/spiffs/db.txt", "w");
  if (!f)
  {
    Serial.println("Error: Could not create db file!");
    return;
  }

  for (int i = 0; i < DB_LENGTH; i++)
  {
    if (db.id[i].length() > 0)
    {
      fprintf(f, "%s;%s;%s\n", db.id[i].c_str(), db.username[i].c_str(), db.password[i].c_str());
    }
  }

  fclose(f);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  Serial.println("Database saved to SPIFFS.");
}

/**
 * @brief Deletes the SPIFFS database file.
 *
 * Removes the database file located at "/spiffs/db.txt" from the SPIFFS
 * filesystem and logs a confirmation message to the serial output.
 */
void delete_spiffs_db()
{
  remove("/spiffs/db.txt");
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  Serial.println("SPIFFS database deleted.");
}

int updateDbLength()
{
  /* Count length */
  FILE *f = fopen("/spiffs/db.txt", "r");
  if (!f)
    return 0;

  int lineCount = 0;
  char line[128];

  while (fgets(line, sizeof(line), f))
  {
    lineCount++;
  }

  fclose(f);
  return lineCount;
}