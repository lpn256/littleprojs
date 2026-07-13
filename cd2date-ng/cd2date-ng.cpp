#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

std::array<std::string, 13> months = {
    "Invalid", "January", "February",  "March",   "April",    "May",     "June",
    "July",    "August",  "September", "October", "November", "December"};

// C type: [A-Y][A-L][0-9][0-9].
int parse_typec(std::string_view hubcode, int &decade, int &year, int &month,
                int &day) {
  year = 0;
  month = 0;
  day = 0;

  for (size_t i = 0; i + 3 <= hubcode.size(); i++) {
    unsigned char c1 = hubcode.at(i);
    unsigned char c2 = hubcode.at(i + 1);
    unsigned char c3 = hubcode.at(i + 2);
    unsigned char c4 = hubcode.at(i + 3);

    if (c1 >= 'A' && c1 <= 'Y' && c2 >= 'A' && c2 <= 'L' && c3 >= '0' &&
        c3 <= '9' && c4 >= '0' && c4 <= '9') {

      int candidate_year, candidate_month, candidate_day;

      // Decade determines base year
      if (decade < 2020) {
        candidate_year = 1995 + (c1 - 'A' + 1);
      } else {
        candidate_year = 2020 + (c1 - 'A' + 1);
      }

      candidate_month = c2 - 'A' + 1;
      candidate_day = (c3 - '0') * 10 + (c4 - '0');

      if (candidate_month >= 1 && candidate_month <= 12 && candidate_day >= 1 &&
          candidate_day <= 31) {
        year = candidate_year;
        month = candidate_month;
        day = candidate_day;
        return 1;
      }
    }
  }
  return 0;
}

// D type: [A-Z][0-9][0-9][0-9][0-9]
int parse_typed(std::string_view hubcode, int &year, int &month, int &day) {
  year = 0;
  month = 0;
  day = 0;

  if (hubcode.length() < 9)
    return 0;

  unsigned char c1 = hubcode.at(1);
  unsigned char c2 = hubcode.at(2);
  unsigned char c3 = hubcode.at(3);
  unsigned char c4 = hubcode.at(7);
  unsigned char c5 = hubcode.at(8);

  if (isupper(c1) && isdigit(c2) && isdigit(c3) && isdigit(c4) && isdigit(c5)) {
    year = 1999 + (c1 - 'A');
    month = (c2 - '0') * 10 + (c3 - '0');
    day = (c4 - '0') * 10 + (c5 - '0');
  }

  return (month >= 1 && month <= 12 && day >= 1 && day <= 31);
}

// Type R: [0-9][0-9][0-9][0-9][0-9]
int parse_typer(std::string_view hubcode, int &decade, int &year, int &month,
                int &day) {
  if (hubcode.size() < 8)
    return 0;

  year = 0;
  month = 0;
  day = 0;

  unsigned char c1 = hubcode.at(3);
  unsigned char c2 = hubcode.at(4);
  unsigned char c3 = hubcode.at(5);
  unsigned char c4 = hubcode.at(6);
  unsigned char c5 = hubcode.at(7);

  if (isdigit(c1) && isdigit(c2) && isdigit(c3) && isdigit(c4) && isdigit(c5)) {
    year = decade + c1 - '0';
    month = (c2 - '0') * 10 + (c3 - '0');
    day = (c4 - '0') * 10 + (c5 - '0');
  }

  return (month >= 1 && month <= 12 && day >= 1 && day <= 31);
}

// T type: [A-Z][0-9][A-L][0-9][0-9]
int parse_typet(std::string hubcode, int &decade, int &year, int &month,
                int &day) {
  if (hubcode.size() < 5)
    return 0;

  year = 0;
  month = 0;
  day = 0;

  unsigned char c1 = hubcode.at(1);
  unsigned char c2 = hubcode.at(2);
  unsigned char c3 = hubcode.at(3);
  unsigned char c4 = hubcode.at(4);

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
  if (hubcode.size() < 4)
    return 0;

  year = 0;
  month = 0;
  day = 0;

  unsigned char c1 = hubcode.at(0);
  unsigned char c2 = hubcode.at(1);
  unsigned char c3 = hubcode.at(2);
  unsigned char c4 = hubcode.at(3);

  if (hubcode.size() >= 4 && isdigit(c1) && isdigit(c2) && isdigit(c3) &&
      isdigit(c4)) {
    std::array<int, 13> days_in_month = {0,  31, 28, 31, 30, 31, 30,
                                         31, 31, 30, 31, 30, 31};
    int y_digit = c1 - '0';
    year = decade + y_digit;

    int doy = (c2 - '0') * 100 + (c3 - '0') * 10 + (c4 - '0');

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
              << " -<manufacturer_type> <hubcode> \"[decade]\"\n"
              << "Manufacturer Types:\n"
              << "  c - CMC Magnetics [A-Z][A-L][0-9][0-9] [decade]\n"
              << "  d - Daxon Technology [A-Z][A-Z][0-9][0-9]...[0-9][0-9]\n"
              << "  r - Type R [0-9][0-9][0-9][0-9][0-9] [decade]-\n"
              << "    Ritek Corporation, MJC Pte Ltd (Singapore),\n"
              << "    Optodisc Technology\n"
              << "  t - TDK Corporation (Luxembourg) [Early]\n"
              << "  g - Generic [0-9][0-9][0-9][0-9] [decade] -\n"
              << "    TDK Corporation (Luxembourg),\n"
              << "    Mitsubishi Chemical Corporation [CD-R],\n"
              << "    Mitsubishi Chemical Corporation [DVD-R], Taiyo Yuden,\n"
              << "    Mitsui Toatsu Chemicals + MAM-E, Moser Baer India,\n"
              << "    Prodisc, Eastman Kodak Company\n\n"
              << "NOTES: Decade defaults to 2000 if not specified.";
    return 1;
  }

  std::string manufacturer_type = argv[1];
  std::string hubcode = argv[2];

  int decade = 2000;

  if (argc >= 4) {
    decade = round(atoi(argv[3]) / 10.0) * 10;
  }

  hubcode.erase(std::remove_if(hubcode.begin(), hubcode.end(),
                               [](unsigned char c) { return std::isspace(c); }),
                hubcode.end());

  int year = 0, month = 0, day = 0;
  int success = 0;

  if (manufacturer_type == "-c") {
    success = parse_typec(hubcode, decade, year, month, day);
  } else if (manufacturer_type == "-d") {
    success = parse_typed(hubcode, year, month, day);
  } else if (manufacturer_type == "-r") {
    success = parse_typer(hubcode, decade, year, month, day);
  } else if (manufacturer_type == "-t") {
    success = parse_typet(hubcode, decade, year, month, day);
  } else if (manufacturer_type == "-g") {
    success = parse_typeg(hubcode, decade, year, month, day);
  }

  if (success) {
    std::cout << months.at(month) << ' ' << day << ", " << year << '\n';
  } else {
    std::cout << "No date string found in the provided hubcode.\n";
  }

  return 0;
}
