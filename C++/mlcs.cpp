#define EXPORT_FCNS
#include <set>
#include <iterator>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <climits>
#include <algorithm>
#include "mlcs.h"


std::map<unsigned long, std::map<std::string, std::set<unsigned long>>> ncsg;
std::vector<std::vector<unsigned long>> sequences;


std::vector<std::vector<unsigned long>> find_all_paths(unsigned long startP, unsigned long endP, unsigned long subCount)
{
	struct stackData
	{
		unsigned long vertex;
		std::vector<unsigned long> path;
	};
	std::vector<std::vector<unsigned long>> paths;
	std::set<unsigned long>::iterator it;
	stackData curPath;
	std::stack<stackData> stack;
	unsigned long vertex;
	std::vector<unsigned long> path;

	curPath.vertex = startP;
	curPath.path = { startP };
	stack.push(curPath);
	while (!stack.empty())
	{
		vertex = stack.top().vertex;
		path = stack.top().path;
		stack.pop();
		for (it = ncsg[vertex]["in"].begin(); it != ncsg[vertex]["in"].end(); it++)
		{
			if (*it == endP)
			{
				paths.push_back(path);
				if ((subCount != 0) & (paths.size() >= subCount))
				{
					return paths;
				}
			}
			else
			{
				curPath.vertex = *it;
				curPath.path = path;
				curPath.path.push_back(*it);
				stack.push(curPath);
			}
		}
	}
	return paths;
}


EXPORTED_FUNCTION short ComputeMCLSs(unsigned short* seqData, unsigned long* seqLength, unsigned long seqNumber, bool preprocessing, unsigned long subCount)
{
	sequences.clear();
	if (seqNumber > 1)
	{
		// Íàõîäèì àëôàâèò ïîñëåäîâàòåëüíîñòåé
		unsigned long idx, jdx, seqItem, shift;
		bool find;
		std::set<unsigned short> alphabet;
		std::set<unsigned short>::iterator it;
		std::set<unsigned short>::iterator tmp;
		for (idx = 0; idx < seqLength[0]; idx++)
		{
			alphabet.insert(seqData[idx]);
		}
		shift = seqLength[0];
		for (seqItem = 1; seqItem < seqNumber; seqItem++)
		{
			for (it = alphabet.begin(); it != alphabet.end(); )
			{
				find = false;
				for (jdx = shift; jdx < (seqLength[seqItem] + shift); jdx++)
				{
					if (seqData[jdx] == *it)
					{
						find = true;
						break;
					}
				}
				if (!find)
				{
					tmp = it;
					++tmp;
					alphabet.erase(it);
					it = tmp;
				}
				else
				{
					++it;
				}
			}
			shift = shift + seqLength[seqItem];
		}

		if (alphabet.size() == 0)	// Åñëè íåò îáùèõ ýëåìåíòîâ âûõîäèì
		{
			return 0;
		}
		else
		{
			unsigned short* preprData;
			unsigned long* preprLength;
			if (preprocessing)    // Ñîçäàåì íîâûå ïîñëåäîâàòåëüíîñòè âñå ýëåìåíòû êîòîðûõ ïåðåñåêàþòñÿ ñ àëôàâèòîì
			{
				preprLength = new unsigned long[seqNumber];
				unsigned long lenData = 0;
				for (seqItem = 0; seqItem < seqNumber; seqItem++) lenData = lenData + seqLength[seqItem];
				preprData = new unsigned short[lenData];
				shift = 0;
				jdx = 0;
				for (seqItem = 0; seqItem < seqNumber; seqItem++)
				{
					preprLength[seqItem] = 0;
					for (idx = shift; idx < (seqLength[seqItem] + shift); idx++)
					{
						if (alphabet.find(seqData[idx]) != alphabet.end())
						{
							preprLength[seqItem] += 1;
							preprData[jdx] = seqData[idx];
							jdx += 1;
						}
					}
					shift = shift + seqLength[seqItem];
				}
			}
			else
			{
				preprData = seqData;
				preprLength = seqLength;
			}

			// Ñîçäàåì òàáëèöû ïðååìíèêè
			std::vector<std::map<unsigned short, std::vector<unsigned long>>> successorTables(seqNumber);
			shift = 0;
			unsigned long pos;
			for (seqItem = 0; seqItem < seqNumber; seqItem++)
			{
				for (idx = shift; idx < (preprLength[seqItem] + shift); idx++)
				{
					for (it = alphabet.begin(); it != alphabet.end(); it++)
					{
						pos = 0;
						for (jdx = idx; jdx < (preprLength[seqItem] + shift); jdx++)
						{
							if (preprData[jdx] == *it)
							{
								pos = jdx - shift + 1;
								break;
							}
						}
						if (pos > 0) successorTables[seqItem][*it].insert(successorTables[seqItem][*it].end(), pos);
					}
				}
				shift = shift + preprLength[seqItem];
			}
			if (preprocessing)
			{
				delete[] preprData;
				delete[] preprLength;
			}

			// Ïîñòðîåíèå ãðàôà NCSG
			ncsg.clear();
			std::vector<unsigned long> sourcePoint(seqNumber);
			std::vector<unsigned long> sinkPoint(seqNumber);
			for (idx = 0; idx < sinkPoint.size(); idx++) sinkPoint[idx] = ULONG_MAX;
			std::set<std::vector<unsigned long>> levelKeys;
			std::set<std::vector<unsigned long>> tmpKeys;
			std::set<std::vector<unsigned long>>::iterator it_levelKeys;
			std::vector<unsigned long> link;
			std::map<std::vector<unsigned long>, unsigned long> pointIndex;
			unsigned long serialNum = 0;
			pointIndex[sourcePoint] = serialNum;
			levelKeys.insert(sourcePoint);
			ncsg[pointIndex[sourcePoint]]["char"].insert({});  ncsg[pointIndex[sourcePoint]]["in"];
			while (levelKeys.size() > 0)
			{
				tmpKeys.clear();
				for (it_levelKeys = levelKeys.begin(); it_levelKeys != levelKeys.end(); it_levelKeys++)
				{
					const std::vector<unsigned long>& keyItem = (*it_levelKeys);
					for (it = alphabet.begin(); it != alphabet.end(); it++)
					{
						link.clear();
						for (seqItem = 0; seqItem < seqNumber; seqItem++)
						{
							if (successorTables[seqItem][*it].size() > keyItem[seqItem])
							{
								link.push_back(successorTables[seqItem][*it][keyItem[seqItem]]);
							}
							else
							{
								link = sinkPoint;
								break;
							}
						}
						if (pointIndex.find(link) == pointIndex.end())
						{
							if (!(std::equal(link.begin(), link.end(), sinkPoint.begin())))
							{
								tmpKeys.insert(link);
							}
							serialNum = serialNum + 1;
							pointIndex[link] = serialNum;
							ncsg[pointIndex[link]]["char"].insert({ *it });
							ncsg[pointIndex[link]]["out"];
						}
						ncsg[pointIndex[link]]["in"].insert(pointIndex[keyItem]);
						ncsg[pointIndex[keyItem]]["out"].insert(pointIndex[link]);
					}

				}
				levelKeys = tmpKeys;
			}

			// Ñîðòèðîâêà ãðàôà ForwardTopSort()
			std::set<unsigned long> levelSerialNum;
			std::set<unsigned long> tmpSerialNum;
			std::set<unsigned long>::iterator it_levelSerialNum;
			std::set<unsigned long>::iterator it_outKeys;
			std::set<unsigned long> difference;
			std::set<unsigned long> unique_outKeys;
			levelSerialNum.insert(pointIndex[sourcePoint]);
			while (levelSerialNum.size() > 0)
			{
				tmpSerialNum.clear();
				unique_outKeys.clear();
				for (it_levelSerialNum = levelSerialNum.begin(); it_levelSerialNum != levelSerialNum.end(); it_levelSerialNum++)
				{
					for (it_outKeys = ncsg[*it_levelSerialNum]["out"].begin(); it_outKeys != ncsg[*it_levelSerialNum]["out"].end(); it_outKeys++)
					{
						unique_outKeys.insert(*it_outKeys);
					}
				}
				for (it_outKeys = unique_outKeys.begin(); it_outKeys != unique_outKeys.end(); )
				{

					difference.clear();
					std::set_difference(ncsg[*it_outKeys]["in"].begin(), ncsg[*it_outKeys]["in"].end(), levelSerialNum.begin(), levelSerialNum.end(), std::inserter(difference, difference.begin()));
					if (difference.size() > 0)
					{
						ncsg[*it_outKeys]["in"] = difference;
						++it_outKeys;
					}
					else
					{
						tmpSerialNum.insert(*it_outKeys);
						++it_outKeys;
					}

				}
				levelSerialNum = tmpSerialNum;
			}

			// ïîëó÷åíèå âñåõ MLCS
			sequences = find_all_paths(pointIndex[sinkPoint], pointIndex[sourcePoint], subCount);
			for (idx = 0; idx < sequences.size(); )
			{
				if (sequences[idx].size() < 2)
				{
					sequences.erase(sequences.begin() + idx);
				}
				else
				{
					sequences[idx].erase(sequences[idx].begin());
					for (jdx = 0; jdx < sequences[idx].size(); jdx++)
					{
						sequences[idx][jdx] = *ncsg[sequences[idx][jdx]]["char"].begin(); // âñå MLCSs çàïèñàíû ðåâåðñèâíî
					}
					idx++;
				}
			}
			if (sequences.size() > 0)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	{
		if (seqNumber == 1)
		{
			sequences.push_back({});
			for (unsigned long idx = 0; idx < seqLength[0]; idx++)
			{
				sequences[0].insert(sequences[0].begin(), seqData[idx]);  // ðåâåðñèâíî ïåðåïèñûâàåì ïåðâûé ýëåìåíò
			}
			return 1;
		}
		else
		{
			return 0;
		}
	}
}


EXPORTED_FUNCTION unsigned short* get_subsequences(unsigned long **sseqLengths, unsigned long *sseqNumber)
{
	*sseqNumber = sequences.size();
	*sseqLengths = new unsigned long[*sseqNumber];
	unsigned long lenData = 0, shift = 0;
	unsigned long idx, jdx;
	for (idx = 0; idx < *sseqNumber; idx++)
	{
		(*sseqLengths)[idx] = sequences[idx].size();
		lenData = lenData + (*sseqLengths)[idx];
	}
	unsigned short* subsequences = new unsigned short[lenData];
	for (idx = 0; idx < *sseqNumber; idx++)
	{
		for (jdx = 0; jdx < (*sseqLengths)[idx]; jdx++)
		{
			subsequences[shift + jdx] = sequences[idx][(*sseqLengths)[idx] - jdx - 1];
		}
		shift = shift + (*sseqLengths)[idx];
	}
	return subsequences;
}

