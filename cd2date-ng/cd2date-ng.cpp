#include <algorithm>
#include <array>
#include <ctype.h>
#include <iostream>
#include <stdlib.h>
#include <string>

std::array<std::string, 13> months = {
    "Invalid", "January", "February",  "March",   "April",    "May",     "June",
    "July",    "August",  "September", "October", "November", "December"};

// C type: [A-Y][A-L][0-9][0-9].
int parse_typec(std::string hubcode, int &year, int &month, int &day) {
  size_t start = 0;
  size_t lh_pos = hubcode.find("LH");

  if (lh_pos != std::string::npos)
    start = lh_pos + 2;

  for (size_t i = start; i + 3 < hubcode.size(); i++) {
    char c1 = hubcode.at(i);
    char c2 = hubcode.at(i + 1);
    char c3 = hubcode.at(i + 2);
    char c4 = hubcode.at(i + 3);

    if (isupper(c1) && isupper(c2) && isdigit(c3) && isdigit(c4)) {
      year = 1996 + (hubcode.at(i) - 'A' + 1);
      month = c2 - 'A' + 1;
      day = (c3 - '0') * 10 + (c4 - '0');
    }

    return (c1 <= 'Y' && month >= 1 && month <= 12 && day >= 1 && day <= 31);
  }
  return 0;
}

// D type: [A-Z][0-9][0-9][0-9][0-9]
int parse_typed(std::string hubcode, int &year, int &month, int &day) {
  char c1 = hubcode.at(1);
  char c2 = hubcode.at(2);
  char c3 = hubcode.at(3);
  char c4 = hubcode.at(7);
  char c5 = hubcode.at(8);

  if (isupper(c1) && isdigit(c2) && isdigit(c3) && isdigit(c4) && isdigit(c5)) {
    year = 1999 + (c1 - 'A');
    month = (c2 - '0') * 10 + (c3 - '0');
    day = (c4 - '0') * 10 + (c5 - '0');
  }

  return (month >= 1 && month <= 12 && day >= 1 && day <= 31);
}

// T type: [0-9][A-L][0-9][0-9]
int parse_typet(std::string hubcode, int &decade, int &year, int &month,
                int &day) {
  char c1 = hubcode.at(1);
  char c2 = hubcode.at(2);
  char c3 = hubcode.at(3);
  char c4 = hubcode.at(4);

  if (hubcode.size() >= 4 && isdigit(c1) && (isdigit(c3)) && (isdigit(c4)) &&
      isupper(c2)) {
    year = 1990 + (c1 - '0');
    month = c2 - 'A' + 1;
    day = (c3 - '0') * 10 + (c4 - '0');
  }

  return (month >= 1 && month <= 12 && day >= 1 && day <= 31);
}

// Generic format (YDDD).
int parse_typeg(std::string hubcode, int &decade, int &year, int &month,
                int &day) {
  char c1 = hubcode.at(0);
  char c2 = hubcode.at(1);
  char c3 = hubcode.at(2);
  char c4 = hubcode.at(3);

  if (hubcode.size() >= 4 && isdigit(c1) && isdigit(c2) && isdigit(c3) &&
      isdigit(c4)) {
    std::array<int, 13> days_in_month = {0,  31, 28, 31, 30, 31, 30,
                                         31, 31, 30, 31, 30, 31};
    int y_digit = c1 - '0';
    year = decade + y_digit;

    int doy = (hubcode.at(1) - '0') * 100 + (hubcode.at(2) - '0') * 10 +
              (hubcode.at(3) - '0');

    if (doy < 1 || doy > 366)
      return 0;

    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
      days_in_month.at(2) = 29;

    while (month <= 12 && doy > days_in_month.at(month)) {
      doy -= days_in_month.at(month);
      month++;
    }

    day = doy; // i spent 30 mins trying to figure out what was wrong here...
  }

  return (month >= 1 && month <= 12 && day >= 1 && day <= 31);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Usage: " << (char *)argv[0]
              << " -<manufacturer_type> <hubcode> [decade]\n"
              << "Manufacturer Types:\n"
              << "  c - CMC Magnetics [A-Z][A-L][0-9][0-9]\n"
              << "  d - Daxon Technology [A-Z][A-Z][0-9][0-9]...[0-9][0-9]\n"
              << "  t - TDK Corporation (Luxembourg) [Early]\n"
              << "  g - Generic YDDD -\n"
              << "    Ritek Corporation, MJC Pte Ltd (Singapore),\n"
              << "    Optodisc Technology, TDK Corporation (Luxembourg),\n"
              << "    Mitsubishi Chemical Corporation [CD-R],\n"
              << "    Mitsubishi Chemical Corporation [DVD-R], Taiyo Yuden,\n"
              << "    Mitsui Toatsu Chemicals + MAM-E, Moser Baer India,\n"
              << "    Prodisc, Eastman Kodak Company\n\n"
              << "NOTES: Decade defaults to 2000 if not specified\n"
              << " (used for standard YDDD format).";
    return 1;
  }

  std::string manufacturer_type = argv[1];
  std::string hubcode = argv[2];

  int decade = 2000;
  if (argc >= 4) {
    decade = atoi(argv[3]);
  }

  hubcode.erase(remove_if(hubcode.begin(), hubcode.end(), isspace),
                hubcode.end());

  int year = 0, month = 0, day = 0;
  int success = 0;

  if (manufacturer_type == "-c") {
    success = parse_typec(hubcode, year, month, day);
  } else if (manufacturer_type == "-d") {
    success = parse_typed(hubcode, year, month, day);
  } else if (manufacturer_type == "-g") {
    success = parse_typeg(hubcode, decade, year, month, day);
  } else if (manufacturer_type == "-t") {
    success = parse_typet(hubcode, decade, year, month, day);
  }

  if (success) {
    std::cout << months.at(month) << ' ' << day << ", " << year << '\n';
  } else {
    std::cout << "No date string found in the provided hubcode.\n";
  }

  return 0;
}
