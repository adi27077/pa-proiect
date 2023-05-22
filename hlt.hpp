#ifndef HLT_H
#define HLT_H

#include <list>
#include <vector>
#include <random>
#include <queue>

#define STILL 0
#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4

using namespace std;

const int DIRECTIONS[] = {STILL, NORTH, EAST, SOUTH, WEST};
const int CARDINALS[] = {NORTH, EAST, SOUTH, WEST};

namespace hlt {
	struct Location {
		unsigned short x, y;
	};
	
	static bool operator<(const Location &l1, const Location &l2) {
		return ((l1.x + l1.y)*((unsigned int) l1.x + l1.y + 1) / 2) + l1.y < ((l2.x + l2.y)*((unsigned int) l2.x + l2.y + 1) / 2) + l2.y;
	}

	struct Site {
		unsigned char owner;
		unsigned char strength;
		unsigned char production;
		unsigned char sourceDirection; //directia catre sursa, pentru flow
		int score; //scorul siteului
		Location location; //locatia siteului
	};
	
	/*comparator pentru priority_queue*/
	class SiteCompare {
	public:
		bool operator() (const Site &s1, const Site &s2) const {
			return s1.score < s2.score;
		}
	};

	class GameMap {
	public:
		vector<vector<Site>> contents;
		unsigned short width, height; //Number of rows & columns, NOT maximum index.

		GameMap() {
			width = 0;
			height = 0;
			contents = vector<vector<Site>>(height, vector<Site>(width, {0, 0, 0}));
		}
		GameMap(const GameMap &otherMap) {
			width = otherMap.width;
			height = otherMap.height;
			contents = otherMap.contents;
		}
		GameMap(int w, int h) {
			width = w;
			height = h;
			contents = vector<vector<Site>>(height, vector<Site>(width, {0, 0, 0}));
		}

		bool inBounds(Location l) {
			return l.x < width && l.y < height;
		}

		float getDistance(Location l1, Location l2) {
			short dx = abs(l1.x - l2.x), dy = abs(l1.y - l2.y);
			if (dx > width / 2) dx = width - dx;
			if (dy > height / 2) dy = height - dy;
			return dx + dy;
		}
		float getAngle(Location l1, Location l2) {
			short dx = l2.x - l1.x, dy = l2.y - l1.y;
			if (dx > width - dx) dx -= width;
			else if (-dx > width + dx) dx += width;
			if (dy > height - dy) dy -= height;
			else if (-dy > height + dy) dy += height;
			return atan2(dy, dx);
		}

		Location getLocation(Location l, unsigned char direction) {
			if (direction != STILL) {
				if (direction == NORTH) {
					if (l.y == 0) l.y = height - 1;
					else l.y--;
				} else if (direction == EAST) {
					if (l.x == width - 1) l.x = 0;
					else l.x++;
				} else if (direction == SOUTH) {
					if (l.y == height - 1) l.y = 0;
					else l.y++;
				} else if (direction == WEST) {
					if (l.x == 0) l.x = width - 1;
					else l.x--;
				}
			}
			return l;
		}

		/*functie care salveaza locatia in interiorul fiecarui site*/
		void initLocations() {
			for (unsigned short i = 0; i < height; i++) {
				for (unsigned short j = 0; j < width; j++) {
					Site &s = getSite({j, i});
					s.location = {j, i};
				}
			}
		}

		Site &getSite(Location l, unsigned char direction = STILL) {
			l = getLocation(l, direction);
			return contents[l.y][l.x];
		}

		/*functie care returneaza inversul unei directii*/
		unsigned char reverseDirection(unsigned char d) {
			switch (d) {
			case NORTH:
				return SOUTH;
				break;
			case SOUTH:
				return NORTH;
				break;
			case WEST:
				return EAST;
				break;
			case EAST:
				return WEST;
				break;
			default:
				return STILL;
			}
		}

		/*functie care seteaza scorurile siteurilor noastre si siteurilor neutre
		de la margine*/
		void setScores(int myID) {
			priority_queue<Site, vector<Site>, SiteCompare> pq;

			//setare scoruri pentru siteurile neutre de la margine
			for (unsigned short i = 0; i < height; i++) {
				for (unsigned short j = 0; j < width; j++) {
					Site &site = getSite({j, i});
					if (site.owner != myID) {
						if (isBorder({j, i}, myID)) {
							//setam scorul conform formulei si adaugam siteul in priority_queue
							site.score = site.production * 5 - (site.strength * 7 / 10) + 50;
							pq.push(site);
						} else {
							site.score = 0;
						}
					}
				}
			}

			//setare scoruri pentru siteurile noastre, pornind de la cele de la margine
			//(aflate in priority_queue). "Curgem" spre interior si setam scorurile conform formulei
			//setam si directia catre sursa pentru fiecare site
			while (!pq.empty()) {
				Site site = pq.top();
				pq.pop();
				for (unsigned char d : CARDINALS) {
					Site &ns = getSite(site.location, d);
					if (ns.owner == myID && !ns.score) {
						ns.score = site.score - ns.production - 2;
						ns.sourceDirection = reverseDirection(d);
						pq.push(ns);
					}
				}
			}
		}

		/* functie care verifica daca un site este la margine (site neutru vecin cu
		un site de al nostru)*/
		bool isBorder(const Location &l, int myID) {
			for (unsigned char d : CARDINALS) {
				Site &ns = getSite(l, d);
				if (ns.owner == myID) {
					return true;
				}
			}
			return false;
		}
	};

	struct Move {
		Location loc; unsigned char dir;
	};

	static bool operator<(const Move &m1, const Move &m2) {
		unsigned int l1Prod = ((m1.loc.x + m1.loc.y)*((unsigned int) m1.loc.x + m1.loc.y + 1) / 2) + m1.loc.y, l2Prod = ((m2.loc.x + m2.loc.y)*((unsigned int) m2.loc.x + m2.loc.y + 1) / 2) + m2.loc.y;
		return ((l1Prod + m1.dir)*(l1Prod + m1.dir + 1) / 2) + m1.dir < ((l2Prod + m2.dir)*(l2Prod + m2.dir + 1) / 2) + m2.dir;
	}
}

#endif
