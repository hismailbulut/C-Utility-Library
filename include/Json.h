#pragma once

#include "StringUtils.h"
#include "containers/Dictionary.h"

/* Creates a json text from given dictionary. */
String JsonCreate(Dictionary* dict);

/* Creates a dictionary from given json text. */
Dictionary* JsonParse(String jsonString);
