#include <vector>
#include <string>
#include <glm/glm.hpp>

#ifndef GREENHOUSEPLANT
#define GREENHOUSEPLANT
class greenHousePlant{
public:
	std::vector<float> boost;
	std::string boostName;
	std::string plantName;
	float level;
	float experience;

	greenHousePlant(std::vector<float> pboost, std::string bName, std::string pName, float plevel, float pexperience)
	: boostName(bName), plantName(pName), level(plevel), experience(pexperience){
		boost.assign(pboost.begin(), pboost.end());
	}

	void addExperience(float exp){
		if(level < 9){
			experience += exp;
			while(experience >= level * 10000){
				experience -= level * 10000;
			}
		}else if(level > 9)
			level = 9;
	}
};
#endif