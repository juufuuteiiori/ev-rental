#ifndef NLP__H
#define NLP__H

#include <iostream>

int getSentiment(const std::string& str, int& result);
int getToxicity(const std::string& str, int& result);

#endif  // NLP_H
