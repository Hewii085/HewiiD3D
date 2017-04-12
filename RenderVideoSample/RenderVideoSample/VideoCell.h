#pragma once
#include<map>

class VideoCell;
class VideoManager
{
public:
	VideoManager() {};
	~VideoManager() {};

	void ReceivedFrame(); //From where?
	void InitVideoCell();
	void SplitCell(int cols, int rows);

private:

	void SplitSub(int cols, int rows);
	std::map<int, VideoCell> m_cellMap;

};
class VideoCell
{
public:
	VideoCell() {};
	~VideoCell() {};
	
	void ReceivedFrame();
	void InitVideoCell();
private:
};