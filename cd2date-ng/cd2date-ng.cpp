#include <algorithm>
#include <array>
#include <ctype.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string_view>

std::array<std::string, 13> months = {
    "Invalid", "January", "February",  "March",   "April",    "May",
    "June", "July", "August",  "September", "October", "November",
    "December"
};

// Leap year fixer.
int is_leap_year(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Day of Year (DoY) to Month and Day.
void yddd_to_date(int year, int doy, int& month, int& day) {
  std::array<int, 13> days_in_month = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
  };

  if (is_leap_year(year))
    days_in_month.at(2) = 29;

  month = 1;
  while (month <= 12 && doy > days_in_month[month]) {
    doy -= days_in_month.at(month);
    month++;
  }

  day = doy; // i spent 30 mins trying to figure out what was wrong here...
}

// C type: [A-Z][A-L][0-9][0-9]. First 5 chars are filler.
int parse_typec(std::string_view hubcode, int& year, int& month, int& day) {
  if (hubcode.size() < 9)
    return 0;

  if (!std::isupper(static_cast<unsigned char>(hubcode.at(5))) ||
    !std::isupper(static_cast<unsigned char>(hubcode.at(6))))
    return 0;

  if (!std::isdigit(static_cast<unsigned char>(hubcode.at(7))) ||
    !std::isdigit(static_cast<unsigned char>(hubcode.at(8))))
    return 0;

  year = 1996 + (hubcode.at(0) - 'A');
  month = hubcode.at(6) - 'A' + 1;
  day = (hubcode.at(7) - '0') * 10 + (hubcode.at(8) - '0');

  return (month >= 1 && month <= 12 && day >= 1 && day <= 31);
}

// D type: [A-Z][A-Z][0-9][0-9]...[0-9][0-9]. GOOD
int parse_typed(std::string_view hubcode, int& year, int& month, int& day) {
  if (hubcode.size() < 9)
    return 0;

  if (!std::isupper(static_cast<unsigned char>(hubcode.at(0))) ||
    !std::isupper(static_cast<unsigned char>(hubcode.at(0))))
    return 0;

  for (size_t i = 2; i < 9; ++i) {
    if (!std::isdigit(static_cast<unsigned char>(hubcode.at(i))))
      return 0;
  }

  year = 1999 + (hubcode.at(1) - 'A');
  month = (hubcode.at(2) - '0') * 10 + (hubcode.at(3) - '0');
  day   = (hubcode.at(7) - '0') * 10 + (hubcode.at(8) - '0');

  return (month >= 1 && month <= 12 && day >= 1 && day <= 31); //is this right?
}

// Generic format (YDDD).
int parse_typeg(std::string_view hubcode, int& decade, int& year, int& month,
                int& day) {
  if (hubcode.size() < 4)
    return 0;

  for (size_t i = 0; i < 4; i++) {
    if (!std::isdigit(static_cast<unsigned char>(hubcode.at(i))))
      return 0;
  }

  int y_digit = hubcode.at(0) - '0';
  year = decade + y_digit;

  int doy = (hubcode.at(1) - '0') * 100 +
    (hubcode.at(2) - '0') * 10 +
    (hubcode.at(3) - '0');

  if (doy < 1 || doy > 366)
    return 0;

  yddd_to_date(year, doy, month, day);
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Usage: " << (char *)argv[0]
              << " -<manufacturer_type> <hubcode> [decade]\n"
              << "Manufacturer Types:\n"
              << "  c - CMC Magnetics [A-Z][A-L][0-9][0-9]\n"
              << "  d - Daxon Technology [A-Z][A-Z][0-9][0-9]...[0-9][0-9]\n"
              << "  g - Generic YDDD -\n"
              << "    Ritek Corporation, MJC Pte Ltd (Singapore),\n"
              << "    Optodisc Technology, TDK Corporation (Luxembourg),\n"
              << "    Mitsubishi Chemical Corporation [CD-R],\n"
              << "    Mitsubishi Chemical Corporation [DVD-R], Taiyo Yuden,\n"
              << "    Mitsui Toatsu Chemicals + MAM-E, Moser Baer India,\n"
              << "    Prodisc, Eastman Kodak Company\n\n"
              << "NOTE: Decade defaults to 2000 if not specified"
              << " (used for standard YDDD format).";
    return 1;
  }

  std::string manufacturer_type = argv[1];
  std::string hubcode = argv[2];

  int decade = 2000;
  if (argc >= 4) {
    decade = atoi(argv[3]);
  }

  hubcode.erase(remove_if(hubcode.begin(), hubcode.end(), isspace), hubcode.end());

  int year = 0, month = 0, day = 0;
  int success = 0;

  if (manufacturer_type == "-c") {
    success = parse_typec(hubcode, year, month, day);
  } else if (manufacturer_type == "-d") {
    success = parse_typed(hubcode, year, month, day);
  } else if (manufacturer_type == "-g") {
    success = parse_typeg(hubcode, decade, year, month, day);
  }

  if (success) {
    std::cout << months.at(month) << " " << day << ", " << year << "\n";
  } else {
    std::cout << "No date string found in the provided hubcode.\n";
  }

  return 0;
}
