#include <cstdlib>
#include <ctime>
#include <set>
#include <fstream>
#include <climits>
#include <queue>
#include <fstream>

#include "hlt.hpp"
#include "networking.hpp"

#define SEED 42
#define MAX_STRENGTH 255

using namespace std;
using namespace hlt;

/* Creeaza o mutare conform regulilor algoritmului si tweakurilor*/
Move getMove(Location location, GameMap &presentMap, int myID) {
	Site s = presentMap.getSite(location);
	//pentru a creea o mutare, verificam daca puterea siteului curent este mai mare decat
	//un multiplu al productiei si daca este mai mare decat puterea maxima
	//daca nu, siteul sta pe loc
	if (s.strength < 6 * s.production && s.strength < MAX_STRENGTH) {
		return {location, STILL};
	}

	Site d = presentMap.getSite(location, s.sourceDirection);
	//cream "flow-ul" mutarilor. Daca siteul destinatie din directia sursa a siteului curent
	//este al nostru sau daca se poate cuceri, cream mutare in directia lui
	if (d.owner == myID || d.strength < s.strength) {
		return {location, s.sourceDirection};
	} else {
		return {location, STILL};
	}
}

int main() {
	srand(SEED);
	cout.sync_with_stdio(0);

	unsigned char myID;
	GameMap presentMap;
	getInit(myID, presentMap);
	sendInit("SigismundDijkstra");

	set<Move> moves;
	while (true) {
		moves.clear();

		getFrame(presentMap);

		presentMap.initLocations();  //adaugare locatie {a, b} pt fiecare site de pe harta
		presentMap.setScores(myID);  //calculeaza scorurile pt siteurile de la margine si cele din interior

		for (unsigned short a = 0; a < presentMap.height; a++) {
			for (unsigned short b = 0; b < presentMap.width; b++) {
				if (presentMap.getSite({b, a}).owner == myID) {
					moves.insert(getMove({b, a}, presentMap, myID));
				}
			}
		}

		sendFrame(moves);
	}

	return 0;
}
