#include <iostream>
#include "include/BencodeToJson.h"


int main() {

	int status = BencodeToJson::BencodeToJSON("sample.torrent");

	return 0;
}