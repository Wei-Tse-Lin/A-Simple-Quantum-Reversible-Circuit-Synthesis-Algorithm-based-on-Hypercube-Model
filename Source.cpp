#include<iostream>
#include<cstring>
#include<string>
#include<cmath>
#include<time.h>
#include <fstream>
#include<sstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <utility>
#include <chrono>
#define DEBUG 1
using namespace std;
const int Dimension = 3;
const int Member = 8;
int Input[Member] = { 0 };
int Output[Member] = {7,6,1,3,4,0,2,5};
int Selectbit_Neighbor[Member][Dimension] = { 0 };//設定每個位置的鄰居
int AHD_Neighbor[Member][Dimension + 1] = { 0 };//設定每個位置自己與鄰居
int Selectbit_Index = 0;//目前全部Selectbit的位置
int Switchbit_Index = 0;//目前Switchbit的位置
int max_HMD_index[Member];//存最大HMD位置
int max_AHD_index[Member];//存最大AHD位置
int after_switch_AHD[Dimension] = { 0 };//交換計算後的AHD
int after_switch_HMD[Dimension] = { 0 };//交換計算後本身的HMD
int after_switch_CTD[Dimension] = { 0 };//交換計算後此Output跟Cycle的距離
int Selectbit_Neighbor_Output[Member][Dimension + 1] = { 0 };//存目前Select bit自己與鄰居上的Output
vector<vector<int> > Total_Cycle_vec(Member);//Cycle表
int Selectbit_in_Cycle_Index[2] = { 0 };
int Switchbit_in_Cycle_Index[2] = { 0 };

string Binary;
fstream file_out;
void Initial()
{
	for (int i = 0; i < Member; i++)
	{
		Input[i] = i;
	}
	for (int i = 0; i < Member; i++)
	{
		for (int j = 0; j < Dimension; j++)
		{
			int a = pow(2, j);
			int b = pow(2, j + 1);
			int tmp = Input[i] % b / a;
			stringstream ss;
			ss << tmp;
			string temp;
			ss >> temp;
			Binary += temp;
			for (int k = 0; k < Dimension; k++)
			{
				int temp = tmp;
				if (j == k) temp = (temp + 1) % 2;
				Selectbit_Neighbor[i][k] += (temp * a);
			}
		}
		if (i != Member - 1)
		{
			Binary += " ";
		}
		sort(Selectbit_Neighbor[i], Selectbit_Neighbor[i] + Dimension);
		for (int j = 0; j < Dimension; j++)
		{
			AHD_Neighbor[i][j + 1] = Selectbit_Neighbor[i][j];
		}
		AHD_Neighbor[i][0] = Input[i];
	}
}
int HMD(int x, int y)//計算目前HMD
{
	int a = x ^ y, count = 0;
	while (a != 0)
	{
		count++;
		a &= a - 1;
	}
	return count;
}
int THD(long long int temp_HMD[])//計算目前THD
{
	int THD = 0;
	for (int i = 0; i < Member; i++)
	{
		THD += temp_HMD[i];
	}
	return THD;
}
void Cycle()
{
	vector<int> temp_cycle_vec;
	vector<pair<int, int> > temp_truth_table_vec;

	temp_truth_table_vec.reserve(Member);
	Total_Cycle_vec.reserve(Member);

	int first_input = 0;
	int cycle_number = 0;

	for (int i = 0; i < Member; i++)
	{
		temp_truth_table_vec.push_back(make_pair(Input[i], Output[i]));
		//	Total_Cycle_vec[i].clear();
	}

	while (!temp_truth_table_vec.empty())
	{
		temp_cycle_vec.clear();

		first_input = temp_truth_table_vec[0].first;

		temp_cycle_vec.push_back(temp_truth_table_vec[0].second);

		Total_Cycle_vec[cycle_number].push_back(temp_cycle_vec.front());

		temp_truth_table_vec.erase(temp_truth_table_vec.begin());

		while (temp_cycle_vec.back() != first_input)
		{
			for (int i = 0; i < temp_truth_table_vec.size(); i++)
			{
				if (temp_cycle_vec.back() == temp_truth_table_vec[i].first)
				{
					temp_cycle_vec.push_back(temp_truth_table_vec[i].second);

					Total_Cycle_vec[cycle_number].push_back(temp_truth_table_vec[i].second);

					temp_truth_table_vec.erase(temp_truth_table_vec.begin() + i);

					break;

				}
			}
		}

		cycle_number++;

	}
#if DEBUG
	for (int i = 0; i < Member; i++)
	{
		if (Total_Cycle_vec[i].size() > 1)
		{
			cout << endl << "目前的Cycle為：";

			for (int j = 0; j < Total_Cycle_vec[i].size(); j++)
			{
				cout << Total_Cycle_vec[i][j] << " ";
			}
		}
	}
#endif
}
void Mix_or_Cut_CycleTable()
{
	bool Select_find = false;
	bool Switch_find = false;
	for (int i = 0; i < Total_Cycle_vec.size(); i++)
	{
		if (Select_find == false)
		{
			auto it = find(Total_Cycle_vec[i].begin(), Total_Cycle_vec[i].end(), Output[Selectbit_Index]); // 找到Selectbit在Total_Cycle_vec裡的位置
			if (it != Total_Cycle_vec[i].end())
			{
				Selectbit_in_Cycle_Index[0] = i;
				Selectbit_in_Cycle_Index[1] = distance(Total_Cycle_vec[i].begin(), it);
				Select_find = true;
			}
		}
		if (Switch_find == false)
		{
			auto it = find(Total_Cycle_vec[i].begin(), Total_Cycle_vec[i].end(), Output[Switchbit_Index]); // 找到要交換的人在Total_Cycle_vec裡的位置
			if (it != Total_Cycle_vec[i].end())
			{
				Switchbit_in_Cycle_Index[0] = i;
				Switchbit_in_Cycle_Index[1] = distance(Total_Cycle_vec[i].begin(), it);
				Switch_find = true;
			}
		}
		if (Select_find == true && Switch_find == true)
		{
			break;
		}
	}

	if (Selectbit_in_Cycle_Index[0] != Switchbit_in_Cycle_Index[0]) //合併Cycle
	{
		if (Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].size() >= Total_Cycle_vec[Switchbit_in_Cycle_Index[0]].size())//若Select的Cycle長度比Switch的Cycle長度長或相等
		{
			vector<int> temp_cycle;

			temp_cycle.reserve(Total_Cycle_vec[Switchbit_in_Cycle_Index[0]].size());

			temp_cycle.assign(Total_Cycle_vec[Switchbit_in_Cycle_Index[0]].begin() + Switchbit_in_Cycle_Index[1], Total_Cycle_vec[Switchbit_in_Cycle_Index[0]].end());

			if (Switchbit_in_Cycle_Index[1] != 0) //若不是從第一個開始
			{
				for (auto it = Total_Cycle_vec[Switchbit_in_Cycle_Index[0]].begin(); it < Total_Cycle_vec[Switchbit_in_Cycle_Index[0]].begin() + Switchbit_in_Cycle_Index[1]; it++)
				{
					temp_cycle.emplace_back(*it);
				}
			}

			Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].insert(Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Selectbit_in_Cycle_Index[1], temp_cycle.begin(), temp_cycle.end());

			Total_Cycle_vec[Switchbit_in_Cycle_Index[0]].clear();

		}
		else if (Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].size() < Total_Cycle_vec[Switchbit_in_Cycle_Index[0]].size())//若Select的Cycle長度比Switch的Cycle長度短
		{
			vector<int> temp_cycle;

			temp_cycle.reserve(Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].size());

			temp_cycle.assign(Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Selectbit_in_Cycle_Index[1], Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].end());

			if (Selectbit_in_Cycle_Index[1] != 0) //若不是從第一個開始
			{
				for (auto it = Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin(); it < Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Selectbit_in_Cycle_Index[1]; it++)
				{
					temp_cycle.emplace_back(*it);
				}
			}

			Total_Cycle_vec[Switchbit_in_Cycle_Index[0]].insert(Total_Cycle_vec[Switchbit_in_Cycle_Index[0]].begin() + Switchbit_in_Cycle_Index[1], temp_cycle.begin(), temp_cycle.end());

			Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].clear();

		}
	}
	else if (Selectbit_in_Cycle_Index[0] == Switchbit_in_Cycle_Index[0])//分割Cycle
	{
		if (Selectbit_in_Cycle_Index[1] < Switchbit_in_Cycle_Index[1])
		{
			for (int i = 0; i < Total_Cycle_vec.size(); i++)
			{
				if (Total_Cycle_vec[i].empty())
				{

					Total_Cycle_vec[i].assign(Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Selectbit_in_Cycle_Index[1], Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Switchbit_in_Cycle_Index[1]);//將Select的Output到Switch的Output中間的元素放到空的Vector裡
					break;
				}
			}
			Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].erase(Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Selectbit_in_Cycle_Index[1], Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Switchbit_in_Cycle_Index[1]);//清除取走的元素
		}
		else if (Switchbit_in_Cycle_Index[1] < Selectbit_in_Cycle_Index[1])
		{
			for (int i = 0; i < Total_Cycle_vec.size(); i++)
			{
				if (Total_Cycle_vec[i].empty())
				{

					Total_Cycle_vec[i].assign(Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Switchbit_in_Cycle_Index[1], Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + (Selectbit_in_Cycle_Index[1]));//將Select的Output到Switch的Output中間的元素放到空的Vector裡
					break;
				}
			}
			Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].erase(Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Switchbit_in_Cycle_Index[1], Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + (Selectbit_in_Cycle_Index[1]));//清除取走的元素
		}
	}

#if DEBUG
	for (int i = 0; i < Member; i++)
	{
		if (Total_Cycle_vec[i].size() > 1)
		{
			cout << endl << "目前的Cycle為：";

			for (int j = 0; j < Total_Cycle_vec[i].size(); j++)
			{
				cout << Total_Cycle_vec[i][j] << " ";
			}
		}
	}
#endif
}
void ECTD(int& Selectbit_Index)
{
	for (int i = 0; i < Dimension; i++)

	{
		after_switch_CTD[i] = 0;
	}

	for (int i = 0; i < Total_Cycle_vec.size(); i++)
	{
		auto it = find(Total_Cycle_vec[i].begin(), Total_Cycle_vec[i].end(), Output[Selectbit_Index]); // 找到Selectbit在Total_Cycle_vec裡的位置
		if (it != Total_Cycle_vec[i].end())
		{
			Selectbit_in_Cycle_Index[0] = i;
			Selectbit_in_Cycle_Index[1] = distance(Total_Cycle_vec[i].begin(), it);
			break;
		}
	}
	for (int i = 0; i < Dimension; i++)
	{
		for (int j = 0; j < Total_Cycle_vec.size(); j++)
		{
			auto it = find(Total_Cycle_vec[j].begin(), Total_Cycle_vec[j].end(), Output[Selectbit_Neighbor[Selectbit_Index][i]]); // 找到要交換的人在Total_Cycle_vec裡的位置

			if (it != Total_Cycle_vec[j].end())
			{
				Switchbit_in_Cycle_Index[0] = j;
				Switchbit_in_Cycle_Index[1] = distance(Total_Cycle_vec[j].begin(), it);
				break;
			}
		}
		if (Selectbit_in_Cycle_Index[0] != Switchbit_in_Cycle_Index[0]) //合併Cycle
		{
			for (auto it = Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin(); it < Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].end(); it++)
			{
				if (*it != Selectbit_Neighbor[Selectbit_Index][i])
				{
					after_switch_CTD[i] += HMD(Selectbit_Neighbor[Selectbit_Index][i], *it);//計算CTD
				}
			}
			for (auto it = Total_Cycle_vec[Switchbit_in_Cycle_Index[0]].begin(); it < Total_Cycle_vec[Switchbit_in_Cycle_Index[0]].end(); it++)
			{
				if (*it != Selectbit_Neighbor[Selectbit_Index][i])
				{
					after_switch_CTD[i] += HMD(Selectbit_Neighbor[Selectbit_Index][i], *it);//計算CTD
				}
			}
		}
		else if (Selectbit_in_Cycle_Index[0] == Switchbit_in_Cycle_Index[0])//分割Cycle
		{
			if (Selectbit_in_Cycle_Index[1] < Switchbit_in_Cycle_Index[1])//算中間的元素
			{
				for (auto it = Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Selectbit_in_Cycle_Index[1]; it < Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Switchbit_in_Cycle_Index[1]; it++)
				{
					if (*it != Selectbit_Neighbor[Selectbit_Index][i])
					{
						after_switch_CTD[i] += HMD(Selectbit_Neighbor[Selectbit_Index][i], *it);
					}
				}
			}
			else if (Switchbit_in_Cycle_Index[1] < Selectbit_in_Cycle_Index[1])//算Select後面的元素
			{
				for (auto it = Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Selectbit_in_Cycle_Index[1]; it < Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].end(); it++)
				{
					if (*it != Selectbit_Neighbor[Selectbit_Index][i])
					{
						after_switch_CTD[i] += HMD(Selectbit_Neighbor[Selectbit_Index][i], *it);
					}
				}
				if (Switchbit_in_Cycle_Index[1] != 0) //加上算Switch前面的元素
				{
					for (auto it = Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin(); it < Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Switchbit_in_Cycle_Index[1]; it++)
					{
						if (*it != Selectbit_Neighbor[Selectbit_Index][i])
						{
							after_switch_CTD[i] += HMD(Selectbit_Neighbor[Selectbit_Index][i], *it);
						}
					}
				}
			}
		}
	}
}
void After_Switch_AHD(long long int old_HMD[])
{
	int temp_HMD[Dimension] = { 0 };
	int temp_output;
	for (int i = 0; i < Dimension; i++)
	{
		after_switch_AHD[i] = 0;
	}
	for (int i = 0; i < Dimension; i++)//目前要交換的bits
	{
		//交換並計算HMD	
		for (int j = 0; j < Dimension; j++)
		{
			if (j != i)
			{
				temp_HMD[j] = old_HMD[Selectbit_Neighbor[Selectbit_Index][j]];
			}
		}
		temp_output = Output[Selectbit_Index];
		Output[Selectbit_Index] = Output[Selectbit_Neighbor[Selectbit_Index][i]];
		Output[Selectbit_Neighbor[Selectbit_Index][i]] = temp_output;

		after_switch_HMD[i] = HMD(Selectbit_Index, Output[Selectbit_Index]);//存本身跟鄰居交換算完的HMD
		temp_HMD[i] = HMD(Selectbit_Neighbor[Selectbit_Index][i], Output[Selectbit_Neighbor[Selectbit_Index][i]]);

		temp_output = Output[Selectbit_Index];
		Output[Selectbit_Index] = Output[Selectbit_Neighbor[Selectbit_Index][i]];
		Output[Selectbit_Neighbor[Selectbit_Index][i]] = temp_output;

		for (int j = 0; j < Dimension; j++)
		{
			after_switch_AHD[i] += temp_HMD[j];
		}
		after_switch_AHD[i] += after_switch_HMD[i];
	}
}
int Max_HMD(long long int temp_HMD[], int count, int& max_HMD, int& max_HMD_number)//計算目前最大HMD與位置
{
	int temp_max = temp_HMD[0];
	for (int i = 0; i < count; i++)
	{
		if (temp_max < temp_HMD[i])
		{
			temp_max = temp_HMD[i];
		}
	}
	max_HMD = temp_max;
	int max_HMD_count = 0;
	for (int i = 0; i < count; i++)
	{
		if (temp_HMD[i] == temp_max)
		{
			max_HMD_index[max_HMD_count] = i;
			max_HMD_number = i;
			max_HMD_count++;
		}
	}
	return max_HMD_count;
}
int AHD(long long int temp_HMD[], int input)//計算AHD
{
	int AHD = 0;
	for (int i = 0; i < Dimension + 1; i++)
	{
		AHD += temp_HMD[AHD_Neighbor[input][i]];
	}
	return AHD;
}
int Max_AHD(long long int temp_AHD[], int count, int& max_AHD, int& max_AHD_number)//計算最大AHD的數量與位置	
{
	int temp_max = temp_AHD[0];
	for (int i = 0; i < count; i++)
	{
		if (temp_max < temp_AHD[i])
		{
			temp_max = temp_AHD[i];
		}
	}
	max_AHD = temp_max;
	int max_AHD_count = 0;
	for (int i = 0; i < count; i++) {
		if (temp_AHD[i] == temp_max) {
			max_AHD_index[max_AHD_count] = max_HMD_index[i];
			max_AHD_number = i;
			max_AHD_count++;
		}
	}
	return max_AHD_count;
}
class Path {
public:
	Path(int A, int C, int H, int PP, int P) :AHD(A), CTD(C), HMD(H), SelectPOS(PP), SwitchPOS(P)
	{}
	int AHD;
	int CTD;
	int HMD;
	int SelectPOS;
	int SwitchPOS;

};
bool Compare_Path(const Path& a, const Path& b)
{
	if (a.AHD < b.AHD)

		return true;

	else if (a.AHD == b.AHD)
	{
		if (a.CTD == b.CTD)
		{
			if (a.HMD == b.HMD)
			{
				return a.SelectPOS < b.SelectPOS;
			}
			return a.HMD < b.HMD;
		}
		return a.CTD < b.CTD;
	}
	else
	{
		return false;
	}
}
void Tabu(vector<pair<int, int> >& pair_dead_path, vector<Path>& path_vec)
{

	if (!pair_dead_path.empty())
	{
		bool dead_path = false;

		for (auto it1 : path_vec)
		{
			for (auto it2 : pair_dead_path)
			{
				if (((it1.SelectPOS == it2.first) && (it1.SwitchPOS == it2.second)) || ((it1.SelectPOS == it2.second) && (it1.SwitchPOS == it2.first)))
				{
					dead_path = true;
					break;
				}
				else
				{
					dead_path = false;
				}
			}
			if (dead_path == true)
			{
				Selectbit_Index = it1.SelectPOS + 1;
				Switchbit_Index = it1.SwitchPOS + 1;
			}
			else if (dead_path == false)
			{
				Selectbit_Index = it1.SelectPOS;
				Switchbit_Index = it1.SwitchPOS;
				break;
			}
		}
	}
}
void Bit_Switch()//使用選擇的路徑交換
{
	Mix_or_Cut_CycleTable();

	int temp_output;
	temp_output = Output[Selectbit_Index];
	Output[Selectbit_Index] = Output[Switchbit_Index];
	Output[Switchbit_Index] = temp_output;

}
void Select_bit_home_and_restore(int Selectbit_Index, int& Select_bit_home_gate_count, long long int temp_HMD[])
{
	for (int i = 0; i < Member; i++)
	{
		temp_HMD[i] = HMD(Input[i], Output[i]);//暫存HMD
	}
	if (temp_HMD[Selectbit_Index] > 0)
	{
		/*---------------------------------------------輸出電路----------------------------------------*/
		/*int x_binary[Dimension];
		int y_binary[Dimension];
		int different = -1;
		for (int i = Dimension - 1; i >= 0; i--)
		{
			int c = pow(2, i);
			int d = pow(2, i + 1);
			int x = Output[Selectbit_Index] % d / c;
			x_binary[Dimension - i - 1] = x;
			int y = Selectbit_Index % d / c;
			y_binary[Dimension - i - 1] = y;
		}
		file_out.open("C:\\Users\\NTU\\Desktop\\小白_可逆電路\\可逆電路實驗數據\\Gate長相.txt", ios::app);
		file_out << endl;
		for (int i = 0; i < Dimension; i++)
		{
			if (x_binary[i] != y_binary[i])
			{
				if (different != -1)
				{
					file_out << endl;
				}
				different = i;
				for (int j = 0; j < i; j++)
				{
					file_out << y_binary[j];
				}
				file_out << 2;
				for (int j = i + 1; j < Dimension; j++)
				{
					file_out << x_binary[j];
				}
			}
		}
		file_out << endl;
		for (int i = different - 1; i >= 0; i--)
		{
			if (x_binary[i] != y_binary[i])
			{
				if (i != different - 1)
				{
					file_out << endl;
				}
				for (int j = 0; j < i; j++)
				{
					file_out << y_binary[j];
				}
				file_out << 2;
				for (int j = i + 1; j < Dimension; j++)
				{
					file_out << x_binary[j];
				}
			}
		}
		file_out.close();*/
		/*---------------------------------------------輸出電路----------------------------------------*/

		for (int i = 0; i < Total_Cycle_vec.size(); i++)
		{
			vector<int>::iterator it = find(Total_Cycle_vec[i].begin(), Total_Cycle_vec[i].end(), Output[Selectbit_Index]); // 找到Selectbit在Total_Cycle_vec裡的位置
			if (it != Total_Cycle_vec[i].end())
			{
				Selectbit_in_Cycle_Index[0] = i;
				Selectbit_in_Cycle_Index[1] = distance(Total_Cycle_vec[i].begin(), it);
				break;
			}
		}


		for (int i = 0; i < Total_Cycle_vec.size(); i++)
		{
			if (Total_Cycle_vec[i].empty())
			{
				Total_Cycle_vec[i].insert(Total_Cycle_vec[i].begin(), Total_Cycle_vec[Selectbit_in_Cycle_Index[0]][Selectbit_in_Cycle_Index[1]]);//將直接回家的Output放到空的Vector裡
				break;
			}
		}

		Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].erase(Total_Cycle_vec[Selectbit_in_Cycle_Index[0]].begin() + Selectbit_in_Cycle_Index[1]);//清除取走的元素

#if DEBUG
		for (int i = 0; i < Member; i++)
		{
			if (Total_Cycle_vec[i].size() > 1)
			{
				cout << endl << "目前的Cycle為：";

				for (int j = 0; j < Total_Cycle_vec[i].size(); j++)
				{
					cout << Total_Cycle_vec[i][j] << " ";
				}
			}

		}
#endif

		Select_bit_home_gate_count += temp_HMD[Selectbit_Index] * 2 - 1;

		int temp_output;
		temp_output = Output[Output[Selectbit_Index]];
		Output[Output[Selectbit_Index]] = Output[Selectbit_Index];
		Output[Selectbit_Index] = temp_output;

	}
	for (int i = 0; i < Member; i++)
	{
		temp_HMD[i] = HMD(Input[i], Output[i]);//暫存HMD
	}
}
void Save_Selectbit_Neighbor_Output()
{
#if DEBUG
	cout << "交換前Select bit " << Selectbit_Index << "自己與鄰居上的Output：";
#endif
	for (int i = 0; i < Dimension + 1; i++)
	{
		Selectbit_Neighbor_Output[Selectbit_Index][i] = Output[AHD_Neighbor[Selectbit_Index][i]];//目前選擇Switch bit鄰居上的Output
	}

	sort(Selectbit_Neighbor_Output[Selectbit_Index], Selectbit_Neighbor_Output[Selectbit_Index] + Dimension + 1);

#if DEBUG
	for (int i = 0; i < Dimension + 1; i++)
	{
		cout << Selectbit_Neighbor_Output[Selectbit_Index][i] << " ";
	}
#endif
}
void Selectbit_same_neighbor_or_not(vector<pair<int, int> >& dead_path, int dead_path_count[])
{
	bool old_path = true;
#if DEBUG
	cout << "比對位置" << Selectbit_Index << "的鄰居上的Output是否相同：";
#endif

	int temp_AHD_Neighbor_Output[Dimension + 1];

	for (int j = 0; j < Dimension + 1; j++)
	{
		temp_AHD_Neighbor_Output[j] = Output[AHD_Neighbor[Selectbit_Index][j]];
	}

	sort(temp_AHD_Neighbor_Output, temp_AHD_Neighbor_Output + Dimension + 1);

	for (int j = 0; j < Dimension + 1; j++) //比對死路前跟死路後選擇Switch bit鄰居上的Output是否一樣
	{
#if DEBUG
		cout << "(" << Selectbit_Neighbor_Output[Selectbit_Index][j] << "," << temp_AHD_Neighbor_Output[j] << ")";
#endif
		if (Selectbit_Neighbor_Output[Selectbit_Index][j] != temp_AHD_Neighbor_Output[j])
		{
			old_path = false;
			break;
		}
	}
	if (old_path == true)//若無新路
	{
		dead_path.push_back(make_pair(Selectbit_Index, Switchbit_Index));//存死路的路徑
#if DEBUG
		cout << endl << "目前共有：" << dead_path.size() << "條死路";
#endif
		dead_path_count[Selectbit_Index]++;
		dead_path_count[Switchbit_Index]++;
	}
}
void Clear_Selectbit_Dead_Path(int Selectbit_Index, int Switchbit_Index, vector<pair<int, int> >& dead_path) //Select bit直接回家時，刪除紀錄該bit死路的路徑
{
	for (int i = 0; i < dead_path.size(); i++)
	{
		if (Selectbit_Index == dead_path[i].first /*|| Switchbit_Index == dead_path[i].first*/)
		{
			dead_path.erase(dead_path.begin() + i);

			i--;
		}
	}
}
void Clear_Selectbit_Repeat_Dead_Path(int Selectbit_Index, int Switchbit_Index, vector<pair<int, int> >& repeat_dead_path) //Select bit直接回家時，刪除紀錄該bit重複禁忌的路徑
{
	for (int i = 0; i < repeat_dead_path.size(); i++)
	{
		if (Selectbit_Index == repeat_dead_path[i].first && Switchbit_Index == repeat_dead_path[i].second || Selectbit_Index == repeat_dead_path[i].second && Switchbit_Index == repeat_dead_path[i].first)
		{
			repeat_dead_path.erase(repeat_dead_path.begin() + i);

			i--;
		}
	}
}
void Clear_Selectbit_Neighbor_Output(vector<pair<int, int> >& dead_path, int dead_path_count[], vector<pair<int, int> >& repeat_dead_path)
{
	if (!dead_path.empty())
	{
#if DEBUG
		cout << endl << "目前的死路為：";
		for (int i = 0; i < dead_path.size(); i++)
		{
			cout << "[" << dead_path[i].first << "," << dead_path[i].second << "]";
		}
#endif
		for (int i = 0; i < dead_path.size(); i++)
		{
			bool old_path = true;
#if DEBUG
			cout << endl << "比對位置" << dead_path[i].first << "的鄰居上的Output是否相同：";
#endif
			int temp_AHD_Neighbor_Output[Dimension + 1];
			for (int j = 0; j < Dimension + 1; j++)
			{
				temp_AHD_Neighbor_Output[j] = Output[AHD_Neighbor[dead_path[i].first][j]];
			}

			sort(temp_AHD_Neighbor_Output, temp_AHD_Neighbor_Output + Dimension + 1);

			for (int j = 0; j < Dimension + 1; j++) //比對死路前跟死路後選擇Switch bit鄰居上的Output是否一樣
			{
#if DEBUG
				cout << "(" << Selectbit_Neighbor_Output[dead_path[i].first][j] << "," << temp_AHD_Neighbor_Output[j] << ")";
#endif
				if (Selectbit_Neighbor_Output[dead_path[i].first][j] != temp_AHD_Neighbor_Output[j])
				{
					old_path = false;
					break;
				}
			}
			if (old_path == false)//若有新路
			{
#if DEBUG
				cout << endl << "因為位置" << dead_path[i].first << "有新路，所以[" << dead_path[i].first << "," << dead_path[i].second << "]解除禁忌";
#endif

				dead_path_count[dead_path[i].first]--;

				dead_path_count[dead_path[i].second]--;

				dead_path.erase(dead_path.begin() + i);

				i--;
			}
		}
	}
}
void Update_Selectbit_Neighbor_Output(vector<pair<int, int> >& all_path)
{
	if (!all_path.empty())
	{
#if DEBUG
		cout << endl << "Select bit " << all_path[0].first << "自己與鄰居上的Output變為：";
#endif
		for (int i = 0; i < Dimension + 1; i++)
		{
			Selectbit_Neighbor_Output[all_path[0].first][i] = Output[AHD_Neighbor[all_path[0].first][i]];//目前選擇Switch bit鄰居上的Output
		}

		sort(Selectbit_Neighbor_Output[all_path[0].first], Selectbit_Neighbor_Output[all_path[0].first] + Dimension + 1);

#if DEBUG
		for (int i = 0; i < Dimension + 1; i++)
		{
			cout << Selectbit_Neighbor_Output[all_path[0].first][i] << " ";
		}
#endif

		for (int i = 1; i < all_path.size(); i++)
		{
			if (all_path[i].first == all_path[i - 1].first)
			{
				continue;
			}
#if DEBUG
			cout << endl << "Select bit " << all_path[i].first << "自己與鄰居上的Output變為：";
#endif
			for (int j = 0; j < Dimension + 1; j++)
			{
				Selectbit_Neighbor_Output[all_path[i].first][j] = Output[AHD_Neighbor[all_path[i].first][j]];//目前選擇Switch bit鄰居上的Output
			}

			sort(Selectbit_Neighbor_Output[all_path[i].first], Selectbit_Neighbor_Output[all_path[i].first] + Dimension + 1);

#if DEBUG
			for (int j = 0; j < Dimension + 1; j++)
			{
				cout << Selectbit_Neighbor_Output[all_path[i].first][j] << " ";
			}
#endif
		}
	}
}
void Read(ifstream& in)//題目
{
	string line;
	for (int i = 0; i < Member; i++)
	{
		in >> Output[i];
	}
}
void Out_Problem(int number, ofstream& out)//輸出
{
	out << "第" << number + 1 << "題" << ",";
	for (int i = 0; i < Member; i++)
	{
		out << Output[i] << " ";
	}
	out << ",";
}
void Out_GateCount(int gate_count, ofstream& out)//輸出
{
	out << gate_count;
	out << ",";
}
void Out_Select_bit_home_gate_count(int Select_bit_home_gate_count, ofstream& out)//輸出
{
	out << Select_bit_home_gate_count;
	out << ",";
}
void Out_Best_GateCount(int best_gate_count, ofstream& out)//輸出
{
	out << best_gate_count;
	out << ",";
}
void Out_Execution_Time(double  execution_time, ofstream& out)//輸出
{
	out << execution_time << endl;
}
void circuit(int a, int b)
{
	for (int j = Dimension - 1; j >= 0; j--)
	{
		int c = pow(2, j);
		int d = pow(2, j + 1);
		int tmp = a % d / c;
		int tmp2 = b % d / c;
		if (tmp == tmp2)
		{
			if (tmp == 0)
			{
				//	cout << "0";
			}
			else
			{
				//	cout << "1";
			}
		}

		//	else cout << "2";

			/*if (j != 0)
			{
				cout << " ";
			}*/
		if (tmp == tmp2)
		{
			if (tmp == 0)
			{
				file_out << "0";
			}
			else
			{
				file_out << "1";
			}
		}
		else file_out << "2";
		if (j != 0)
		{
			//	file_out << " ";
		}
	}
	//cout << endl;
}
int main()
{
	Initial();
	//ifstream in("C:\\Users\\NTU\\Desktop\\小白_可逆電路\\可逆題目\\nth_prime14_inc.txt");
	//Read(in);
	vector<Path> path_vec;
	vector<pair<int, int> > pair_path_vec;
	vector<vector<int> > output_vec(Member);
	int max_HMD, max_index_HMD, max_HMD_count;
	int max_AHD, max_index_AHD, max_AHD_count;
	int min_HMD, min_index_HMD, min_HMD_count;
	int min_AHD, min_index_AHD, min_AHD_count;
	long long int temp_HMD[Member] = { 0 };
	long long int temp_AHD[Member] = { 0 };
	int temp_output = 0;
	int gate_count = 0;
	int repeat_gate_count = 0;
	int Select_bit_home_gate_count = 0;
	int best_gate_count = 0;
	int n = 1;
	vector<pair<int, int> > pair_dead_path;
	vector<pair<int, int> > repeat_dead_path;
	int dead_path_count[Member] = { 0 };

#if DEBUG
	cout << "目前的Output：     ";
	for (int i = 0; i < Member; i++)
	{

		cout << Output[i] << " ";//印出目前Output

	}
	cout << endl << "目前每個位置的HMD：";
#endif
	for (int i = 0; i < Member; i++)
	{
		temp_HMD[i] = HMD(Input[i], Output[i]);//暫存HMD
#if DEBUG
		cout << temp_HMD[i] << " ";//輸出目前每個位置的HMD
#endif
	}
	auto Start = chrono::steady_clock::now();
	Cycle();
	while (THD(temp_HMD) != 0 /*&& gate_count < 100*/)
	{
	Goto:
		bool Not_Only_MaxHMD = false;
		bool Select_home = false;
#if DEBUG ==0
		THD(temp_HMD);
#else
		cout << endl << "目前的THD：" << THD(temp_HMD);
#endif
		Clear_Selectbit_Neighbor_Output(pair_dead_path, dead_path_count, repeat_dead_path);

		max_HMD_count = Max_HMD(temp_HMD, Member, max_HMD, max_index_HMD);//計算最大HMD

#if DEBUG
		cout << endl << "目前最大HMD：" << max_HMD << endl;
#endif
		for (int i = 0; i < max_HMD_count; i++)
		{
#if DEBUG
			cout << "目前最大HMD位置：" << max_HMD_index[i] << endl;
#endif
		}
#if DEBUG
		cout << "目前最大HMD數量：" << max_HMD_count << "個" << endl;
#endif
		if (max_HMD_count == 1)//若最大HMD只有一個
		{
			Selectbit_Index = max_HMD_index[0];
			path_vec.clear();
			After_Switch_AHD(temp_HMD);//直接計算要交換的Switch Bit路徑


			ECTD(Selectbit_Index);//計算交換後與Cylce的距離


			for (int i = 0; i < Dimension; i++)
			{
				Path p(after_switch_AHD[i], after_switch_CTD[i], after_switch_HMD[i], Selectbit_Index, Selectbit_Neighbor[Selectbit_Index][i]);
				path_vec.push_back(p);
			}

			sort(path_vec.begin(), path_vec.end(), Compare_Path);//排序計算後的路徑

#if DEBUG
			cout << "計算要交換的Switch Bit路徑(AHD, CTD, HMD, 自身位置, 交換位置)：";

			for (int i = 0; i < Dimension; i++)
			{
				cout << "(" << path_vec[i].AHD << "," << path_vec[i].CTD << "," << path_vec[i].HMD << "," << path_vec[i].SelectPOS << "," << path_vec[i].SwitchPOS << ")";
			}
#endif
			Switchbit_Index = path_vec[0].SwitchPOS;//選排序後的最佳路徑

			bool dead_path = false;

			Tabu(pair_dead_path, path_vec);

		}
		else //若最大HMD不只一個，則全部計算路徑
		{
			Not_Only_MaxHMD = true;

			path_vec.clear();


			for (int i = 0; i < max_HMD_count; i++)
			{

				int temp2_HMD[Dimension];
				int temp_output;

				for (int j = 0; j < Dimension; j++)
				{
					after_switch_AHD[j] = 0;
				}

				for (int j = 0; j < Dimension; j++)//目前要交換的bits
				{
					//交換並計算HMD	
					for (int k = 0; k < Dimension; k++)
					{
						if (k != j)
						{
							temp2_HMD[k] = temp_HMD[Selectbit_Neighbor[max_HMD_index[i]][k]];
						}
					}

					temp_output = Output[max_HMD_index[i]];
					Output[max_HMD_index[i]] = Output[Selectbit_Neighbor[max_HMD_index[i]][j]];
					Output[Selectbit_Neighbor[max_HMD_index[i]][j]] = temp_output;

					after_switch_HMD[j] = HMD(max_HMD_index[i], Output[max_HMD_index[i]]);//存本身跟鄰居交換算完的HMD
					temp2_HMD[j] = HMD(Selectbit_Neighbor[max_HMD_index[i]][j], Output[Selectbit_Neighbor[max_HMD_index[i]][j]]);

					temp_output = Output[max_HMD_index[i]];
					Output[max_HMD_index[i]] = Output[Selectbit_Neighbor[max_HMD_index[i]][j]];
					Output[Selectbit_Neighbor[max_HMD_index[i]][j]] = temp_output;

					for (int k = 0; k < Dimension; k++)
					{
						after_switch_AHD[j] += temp2_HMD[k];
					}

					after_switch_AHD[j] += after_switch_HMD[j];
				}

				ECTD(max_HMD_index[i]);//計算交換後與Cylce的距離

				for (int j = 0; j < Dimension; j++)
				{
					Path p(after_switch_AHD[j], after_switch_CTD[j], after_switch_HMD[j], max_HMD_index[i], Selectbit_Neighbor[max_HMD_index[i]][j]);
					path_vec.push_back(p);
				}

			}

			sort(path_vec.begin(), path_vec.end(), Compare_Path);//排序計算後的路徑

#if DEBUG
			cout << "計算要交換的Switch Bit路徑(AHD, CTD, HMD, 自身位置, 交換位置)：";

			for (int i = 0; i < Dimension * max_HMD_count; i++)
			{
				cout << "(" << path_vec[i].AHD << "," << path_vec[i].CTD << "," << path_vec[i].HMD << "," << path_vec[i].SelectPOS << "," << path_vec[i].SwitchPOS << ")";
			}
#endif

			Selectbit_Index = path_vec[0].SelectPOS;

			Switchbit_Index = path_vec[0].SwitchPOS;

			Tabu(pair_dead_path, path_vec);
		}

		/*----------------------------------------------------------交換SwitchBit-------------------------------------------------------------------------*/

		/*----------------------------------------------------------判斷是否死路-------------------------------------------------------------------------*/

		if (!output_vec[Selectbit_Index].empty() && !output_vec[Switchbit_Index].empty())
		{
			if (output_vec[Selectbit_Index].back() == Output[Switchbit_Index] && output_vec[Switchbit_Index].back() == Output[Selectbit_Index])//重複判斷Output是否有交叉
			{
				for (int p = pair_path_vec.size() - 1; p >= 0; p--)
				{
					if (Selectbit_Index == pair_path_vec[p].second && Switchbit_Index == pair_path_vec[p].first)//交叉死路
					{
#if DEBUG
						cout << endl << "目前要交換的Switch Bit路徑：" << Switchbit_Index << endl;
						cout << "第" << n++ << "次選擇的路徑位置：(" << Selectbit_Index << "," << Switchbit_Index << ")" << endl;
						cout << "因為" << "(" << pair_path_vec[p].first << "," << pair_path_vec[p].second << ")" << "(" << Selectbit_Index << "," << Switchbit_Index << ")" << "發生交叉死路：" << endl;
#endif
						int temp;
						temp = Selectbit_Index;
						Selectbit_Index = Switchbit_Index;
						Switchbit_Index = temp;
						repeat_gate_count += 2;
						gate_count++;
#if DEBUG
						cout << "目前共有：" << repeat_gate_count << "個重複的Gates" << endl;
#endif

						Bit_Switch();//使用選擇的路徑交換

						Selectbit_same_neighbor_or_not(pair_dead_path, dead_path_count);


						if (dead_path_count[Selectbit_Index] >= Dimension)
						{
#if DEBUG
							cout << endl << "因Select bit " << Selectbit_Index << " 全部被禁忌，故讓Output " << Output[Selectbit_Index] << " 直接回家並還原，使用了 " << temp_HMD[Selectbit_Index] * 2 - 1 << " 個Gates";
# endif
							Select_bit_home_and_restore(Selectbit_Index, Select_bit_home_gate_count, temp_HMD);

							Clear_Selectbit_Dead_Path(Selectbit_Index, Switchbit_Index, pair_dead_path);//因Select bit直接回家，刪除紀錄該bit死路的路徑

							Clear_Selectbit_Repeat_Dead_Path(Selectbit_Index, Switchbit_Index, repeat_dead_path);//因Select bit直接回家，刪除紀錄該bit重複禁忌的路徑

							Update_Selectbit_Neighbor_Output(pair_path_vec);

							dead_path_count[Selectbit_Index] = 0;

							Select_home = true;
						}

						if (dead_path_count[Switchbit_Index] >= Dimension)
						{

#if DEBUG
							cout << endl << "因Select bit " << Switchbit_Index << " 全部被禁忌，故讓Output " << Output[Switchbit_Index] << " 直接回家並還原，使用了 " << temp_HMD[Switchbit_Index] * 2 - 1 << " 個Gates";
# endif
							Select_bit_home_and_restore(Switchbit_Index, Select_bit_home_gate_count, temp_HMD);

							Clear_Selectbit_Dead_Path(Switchbit_Index, Selectbit_Index, pair_dead_path);//因Select bit直接回家，刪除紀錄該bit死路的路徑

							Clear_Selectbit_Repeat_Dead_Path(Switchbit_Index, Selectbit_Index, repeat_dead_path);//因Select bit直接回家，刪除紀錄該bit重複禁忌的路徑

							Update_Selectbit_Neighbor_Output(pair_path_vec);

							dead_path_count[Switchbit_Index] = 0;


							Select_home = true;
						}
						if (!repeat_dead_path.empty())
						{
							for (int d = repeat_dead_path.size() - 1; d >= 0; d--)
							{
								if (Selectbit_Index == repeat_dead_path[d].first && Switchbit_Index == repeat_dead_path[d].second || Selectbit_Index == repeat_dead_path[d].second && Switchbit_Index == repeat_dead_path[d].first)
								{
#if DEBUG
									cout << endl << "因Select bit " << Selectbit_Index << " 重複被禁忌，故讓Output " << Output[Selectbit_Index] << " 直接回家並還原，使用了 " << temp_HMD[Selectbit_Index] * 2 - 1 << " 個Gates";
# endif
									Select_bit_home_and_restore(Selectbit_Index, Select_bit_home_gate_count, temp_HMD);

									dead_path_count[Selectbit_Index] = 0;

									Clear_Selectbit_Dead_Path(Selectbit_Index, Switchbit_Index, pair_dead_path);//因Select bit直接回家，刪除紀錄該bit死路的路徑

									Clear_Selectbit_Repeat_Dead_Path(Selectbit_Index, Switchbit_Index, repeat_dead_path);//因Select bit直接回家，刪除紀錄該bit重複禁忌的路徑

									Update_Selectbit_Neighbor_Output(pair_path_vec);

									Select_home = true;

									break;
								}
							}
						}

						/*-----------------------------輸出電路-----------------------------*/
#if DEBUG
	                                        // file_out.open("C:\\Users\\NTU\\Desktop\\ppt\\可逆電路\\實驗數據\\Gate長相.txt", ios::app);

						cout << endl << "Gate長相：";

						//		file_out << endl;
						circuit(Selectbit_Index, Switchbit_Index);
						//	file_out.close();
#endif
						/*-----------------------------輸出電路-----------------------------*/

#if DEBUG
						cout << endl << "交換後的Output：   ";

						for (int i = 0; i < Member; i++)
						{
							cout << Output[i] << " ";//印出交換後的Output
						}
						cout << endl;

						cout << endl << "目前每個位置的HMD：";
#endif
						for (int i = 0; i < Member; i++)
						{
							temp_HMD[i] = HMD(Input[i], Output[i]);//暫存HMD
#if DEBUG
							cout << temp_HMD[i] << " ";//輸出目前每個位置的HMD
#endif
						}
						if (Select_home == false)
						{
							repeat_dead_path.push_back(make_pair(Selectbit_Index, Switchbit_Index));//存第一次發生死路的路徑
						}
						pair_path_vec.erase(pair_path_vec.begin() + p);
						output_vec[Selectbit_Index].pop_back();
						output_vec[Switchbit_Index].pop_back();

						goto Goto;
					}

					else if (Selectbit_Index == pair_path_vec[p].first && Switchbit_Index == pair_path_vec[p].second)//平行死路
					{
#if DEBUG
						cout << endl << "目前要交換的Switch Bit路徑：" << Switchbit_Index << endl;
						cout << "第" << n++ << "次選擇的路徑位置：(" << Selectbit_Index << "," << Switchbit_Index << ")" << endl;
						cout << "因為" << "(" << pair_path_vec[p].first << "," << pair_path_vec[p].second << ")" << "(" << Selectbit_Index << "," << Switchbit_Index << ")" << "發生平行死路：" << endl;
#endif
						repeat_gate_count += 2;
						gate_count++;
#if DEBUG
						cout << "目前共有：" << repeat_gate_count << "個重複的Gates" << endl;
#endif

						Bit_Switch();//使用選擇的路徑交換

						/*-----------------------------輸出電路-----------------------------*/
						/*file_out.open("C:\\Users\\NTU\\Desktop\\小白_可逆電路\\可逆電路實驗數據\\Gate長相.txt", ios::app);


						//	cout << endl << "Gate長相：";

						file_out << endl;

						circuit(Selectbit_Index, Switchbit_Index);

						file_out.close();*/
						/*-----------------------------輸出電路-----------------------------*/

						Selectbit_same_neighbor_or_not(pair_dead_path, dead_path_count);

						if (dead_path_count[Selectbit_Index] >= Dimension)
						{
#if DEBUG
							cout << endl << "因Select bit " << Selectbit_Index << " 全部被禁忌，故讓Output " << Output[Selectbit_Index] << " 直接回家並還原，使用了 " << temp_HMD[Selectbit_Index] * 2 - 1 << " 個Gates";
# endif
							Select_bit_home_and_restore(Selectbit_Index, Select_bit_home_gate_count, temp_HMD);

							Clear_Selectbit_Dead_Path(Selectbit_Index, Switchbit_Index, pair_dead_path);//因Select bit直接回家，刪除紀錄該bit死路的路徑

							Clear_Selectbit_Repeat_Dead_Path(Selectbit_Index, Switchbit_Index, repeat_dead_path);//因Select bit直接回家，刪除紀錄該bit重複禁忌的路徑

							Update_Selectbit_Neighbor_Output(pair_path_vec);

							dead_path_count[Selectbit_Index] = 0;

							Select_home = true;

						}

						if (dead_path_count[Switchbit_Index] >= Dimension)
						{

#if DEBUG
							cout << endl << "因Select bit " << Switchbit_Index << " 全部被禁忌，故讓Output " << Output[Switchbit_Index] << " 直接回家並還原，使用了 " << temp_HMD[Switchbit_Index] * 2 - 1 << " 個Gates";
# endif
							Select_bit_home_and_restore(Switchbit_Index, Select_bit_home_gate_count, temp_HMD);

							Clear_Selectbit_Dead_Path(Switchbit_Index, Selectbit_Index, pair_dead_path);//因Select bit直接回家，刪除紀錄該bit死路的路徑

							Clear_Selectbit_Repeat_Dead_Path(Switchbit_Index, Selectbit_Index, repeat_dead_path);//因Select bit直接回家，刪除紀錄該bit重複禁忌的路徑

							Update_Selectbit_Neighbor_Output(pair_path_vec);

							dead_path_count[Switchbit_Index] = 0;

							Select_home = true;
						}
						if (!repeat_dead_path.empty())
						{
							for (int d = repeat_dead_path.size() - 1; d >= 0; d--)
							{
								if (Selectbit_Index == repeat_dead_path[d].first && Switchbit_Index == repeat_dead_path[d].second || Selectbit_Index == repeat_dead_path[d].second && Switchbit_Index == repeat_dead_path[d].first)
								{
#if DEBUG
									cout << endl << "因Select bit " << Selectbit_Index << " 重複被禁忌，故讓Output " << Output[Selectbit_Index] << " 直接回家並還原，使用了 " << temp_HMD[Selectbit_Index] * 2 - 1 << " 個Gates";
# endif
									Select_bit_home_and_restore(Selectbit_Index, Select_bit_home_gate_count, temp_HMD);

									dead_path_count[Selectbit_Index] = 0;

									Clear_Selectbit_Dead_Path(Selectbit_Index, Switchbit_Index, pair_dead_path);//因Select bit直接回家，刪除紀錄該bit死路的路徑

									Clear_Selectbit_Repeat_Dead_Path(Selectbit_Index, Switchbit_Index, repeat_dead_path);//因Select bit直接回家，刪除紀錄該bit重複禁忌的路徑

									Update_Selectbit_Neighbor_Output(pair_path_vec);

									Select_home = true;

									break;
								}
							}
						}

						/*-----------------------------輸出電路-----------------------------*/
#if DEBUG
			                    //  file_out.open("C:\\Users\\NTU\\Desktop\\ppt\\可逆電路\\實驗數據\\Gate長相.txt", ios::app);

						cout << endl << "Gate長相：";

						//		file_out << endl;
						circuit(Selectbit_Index, Switchbit_Index);
						//	file_out.close();
#endif
						/*-----------------------------輸出電路-----------------------------*/

#if DEBUG
						cout << endl << "交換後的Output：   ";

						for (int i = 0; i < Member; i++)
						{
							cout << Output[i] << " ";//印出交換後的Output
						}
						cout << endl;

						cout << endl << "目前每個位置的HMD：";
#endif
						for (int i = 0; i < Member; i++)
						{
							temp_HMD[i] = HMD(Input[i], Output[i]);//暫存HMD
#if DEBUG
							cout << temp_HMD[i] << " ";//輸出目前每個位置的HMD
#endif
						}

						if (Select_home == false)
						{
							repeat_dead_path.push_back(make_pair(Selectbit_Index, Switchbit_Index));//存第一次發生死路的路徑
						}

						pair_path_vec.erase(pair_path_vec.begin() + p);
						output_vec[Selectbit_Index].pop_back();
						output_vec[Switchbit_Index].pop_back();

						goto Goto;

					}
				}
			}
		}

		/*----------------------------------------------------------判斷是否死路-------------------------------------------------------------------------*/
#if DEBUG
		cout << endl << "目前要與Switch Bit交換的路徑：" << Switchbit_Index;
		cout << endl << "第" << n++ << "次選擇的路徑位置：(" << Selectbit_Index << "," << Switchbit_Index << ")" << endl;
		cout << "目前共有：" << repeat_gate_count << "個重複的Gates" << endl;
#endif
		pair_path_vec.push_back(make_pair(Selectbit_Index, Switchbit_Index));//存上次的路徑
		output_vec[Selectbit_Index].push_back(Output[Selectbit_Index]);//存上次的路徑的Output
		output_vec[Switchbit_Index].push_back(Output[Switchbit_Index]);//存上次的路徑的Output

		if (Not_Only_MaxHMD == false)
		{
			Save_Selectbit_Neighbor_Output();
		}
		else
		{
			for (unsigned int i = 0; i < max_HMD_count; i++)
			{
#if DEBUG
				cout << "交換前Select bit " << max_HMD_index[i] << "自己與鄰居上的Output：";
#endif

				for (unsigned int j = 0; j < Dimension + 1; j++)
				{
					Selectbit_Neighbor_Output[max_HMD_index[i]][j] = Output[AHD_Neighbor[max_HMD_index[i]][j]];//目前選擇Switch bit鄰居上的Output
				}


				sort(Selectbit_Neighbor_Output[max_HMD_index[i]], Selectbit_Neighbor_Output[max_HMD_index[i]] + Dimension + 1);

#if DEBUG

				for (unsigned int j = 0; j < Dimension + 1; j++)
				{
					cout << Selectbit_Neighbor_Output[max_HMD_index[i]][j] << " ";
				}
#endif
			}
		}

		Bit_Switch();//使用選擇的路徑交換

	        /*-----------------------------輸出電路-----------------------------*/
	/*	if (temp_HMD[Selectbit_Index] != 0)
		{
			file_out.open("C:\\Users\\NTU\\Desktop\\小白_可逆電路\\可逆電路實驗數據\\Gate長相.txt", ios::app);
		//	cout << endl << "Gate長相：";
			file_out << endl;
			circuit(Selectbit_Index, Switchbit_Index);
			file_out.close();
		}*/
		/*-----------------------------輸出電路-----------------------------*/

#if DEBUG
		cout << endl << "交換後的Output：   ";
		for (int i = 0; i < Member; i++)
		{
			cout << Output[i] << " ";//印出交換後的Output
		}
#endif
#if DEBUG
		cout << endl << "目前每個位置的HMD：";
#endif
		for (int i = 0; i < Member; i++)
		{
			temp_HMD[i] = HMD(Input[i], Output[i]);//暫存HMD
#if DEBUG
			cout << temp_HMD[i] << " ";//輸出目前每個位置的HMD
#endif
		}
		gate_count++;
	}
	best_gate_count = gate_count - repeat_gate_count + Select_bit_home_gate_count;
	auto End = chrono::steady_clock::now();
	cout << endl << "總共使用：" << gate_count + Select_bit_home_gate_count << "個Gates";
	cout << endl << "遇到死路直接回家：" << Select_bit_home_gate_count << "個Gates";
	cout << endl << "共有：" << repeat_gate_count << "個重複的Gates";
	cout << endl << "最佳解為：" << best_gate_count << "個Gates" << endl;
	cout << endl << "進行運算所花費的時間：" << chrono::duration_cast<chrono::nanoseconds>(End - Start).count() << " ns" << endl;
	cout << endl << "----------------------------------------------" << endl;
	return 0;
}
