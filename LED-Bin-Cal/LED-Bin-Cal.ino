#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SD.h>

// WiFi credentials
const char* ssid = "Test_Wlan5G";
const char* password = "hGz38jk5E";

// NTP settings
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600; // Offset in seconds, adjust according to your timezone
const int daylightOffset_sec = 3600; // Daylight offset in seconds, adjust according to your timezone

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec + daylightOffset_sec);

// Function to normalize CSV file content
void normalizeCSV(const char* inputPath, const char* outputPath) {
  File inputFile = SD.open(inputPath, FILE_READ);
  if (!inputFile) {
    Serial.println("Failed to open input file");
    return;
  }

  File outputFile = SD.open(outputPath, FILE_WRITE);
  if (!outputFile) {
    Serial.println("Failed to open output file");
    inputFile.close();
    return;
  }

  while (inputFile.available()) {
    String line = inputFile.readStringUntil('\n');
    line.replace(',', ';'); // Ensure delimiter is a semicolon

    // Remove any additional spaces
    line.trim();

    // Skip empty lines
    if (line.length() == 0) {
      continue;
    }

    // Write the normalized line to the output file
    outputFile.println(line);
  }

  inputFile.close();
  outputFile.close();
}

// Function to parse CSV line and extract the next trash type for tomorrow's date
String getNextTrashType(const char* filePath) {
  File file = SD.open(filePath, FILE_READ);

  if (!file) {
    Serial.println("Failed to open file");
    return "";
  }

  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();

  if (epochTime < 1000000000) { // Check if the epoch time is reasonable (after 2001)
    Serial.println("Invalid epoch time received.");
    file.close();
    return "";
  }

  struct tm *timeInfo;
  time_t localTime = epochTime + gmtOffset_sec;
  timeInfo = localtime(&localTime);

  // Increment day by one
  timeInfo->tm_mday += 1;

  // Use mktime() to handle month/year boundaries
  time_t futureEpochTime = mktime(timeInfo);

  char dateString[11];
  strftime(dateString, sizeof(dateString), "%d.%m.%Y", localtime(&futureEpochTime));
  String targetDate = String(dateString);

  String trashType = "";

  // Read the first row to get the headers (trash types)
  String headerLine = file.readStringUntil('\n');
  if (headerLine.length() == 0) {
    file.close();
    return "";
  }
  headerLine.trim();
  headerLine.replace(';', ',');
  String headers[10];
  int headerCount = 0;

  int startIndex = 0;
  while (startIndex < headerLine.length()) {
    int commaIndex = headerLine.indexOf(',', startIndex);
    if (commaIndex == -1) {
      headers[headerCount++] = headerLine.substring(startIndex);
      break;
    }
    headers[headerCount++] = headerLine.substring(startIndex, commaIndex);
    startIndex = commaIndex + 1;
  }

  // Iterate through the file line by line
  while (file.available()) {
    String line = file.readStringUntil('\n');
    if (line.length() == 0) {
      continue; // Skip empty lines
    }

    line.trim();
    line.replace(';', ',');
    
    // Split the line by commas
    int commaIndex = line.indexOf(',');
    String date = line.substring(0, commaIndex);
    date.trim();

    if (date == targetDate) {
      startIndex = commaIndex + 1;
      int columnIndex = 0;
      while (startIndex < line.length()) {
        commaIndex = line.indexOf(',', startIndex);
        String token;
        if (commaIndex == -1) {
          token = line.substring(startIndex);
        } else {
          token = line.substring(startIndex, commaIndex);
        }
        token.trim();

        if (token == "1") {
          trashType = headers[columnIndex];
          if (trashType == "Papiertonne") {
            file.close();
            return trashType;
          }
        }

        if (commaIndex == -1) {
          break;
        }
        startIndex = commaIndex + 1;
        columnIndex++;
      }
    }
  }

  // Close file after processing
  file.close();

  // If target date is found, return the trash type
  return trashType;
}

void printLocalTime(unsigned long epochTime) {
  struct tm *timeInfo;
  time_t localTime = epochTime + gmtOffset_sec;
  timeInfo = localtime(&localTime);

  Serial.print("Local Time: ");
  Serial.print(asctime(timeInfo));
}

void setup() {
  Serial.begin(115200);
  if (!SD.begin(D8)) { // Adjust pin if needed
    Serial.println("SD Card Mount Failed");
    return;
  }

  // Normalize the CSV file
  normalizeCSV("/bruehlstroberndorf.csv", "/normalized.csv");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // Initialize time client and sync time
  timeClient.begin();
  timeClient.update();

  unsigned long epochTime = timeClient.getEpochTime();
  if (epochTime < 1000000000) { // Check if the epoch time is reasonable (after 2001)
    Serial.println("Invalid epoch time received.");
    return;
  }

  // Debug print for epoch time
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);

  // Debug print for current local time
  printLocalTime(epochTime);

  String trashType = getNextTrashType("/normalized.csv");
  Serial.print("Next trash type: ");
  Serial.println(trashType);
}

void loop() {
  static unsigned long lastUpdate = 0;

  // Update time from NTP server every hour
  if (millis() - lastUpdate > 3600000) {
    timeClient.update();
    lastUpdate = millis();

    // Debug prints for NTP time in loop
    unsigned long epochTime = timeClient.getEpochTime();
    if (epochTime < 1000000000) { // Check if the epoch time is reasonable (after 2001)
      Serial.println("Invalid epoch time received.");
      return;
    }

    // Debug print for epoch time
    Serial.print("Epoch Time: ");
    Serial.println(epochTime);

    // Debug print for current time
    struct tm timeInfo;
    gmtime_r((time_t *)&epochTime, &timeInfo);
    Serial.print("Current GMT time: ");
    Serial.print(asctime(&timeInfo));
  }

  // You can add other functionalities here if needed
}
