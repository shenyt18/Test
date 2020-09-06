#include<iostream>
#include<limits.h>
using std::cin;
using std::cout;
using std::endl;

struct point
{
	int position;																			//储存此点为图中的哪个店
	int former;																				//储存此点前一个点的队列信息
};
point queue[2005];
bool PT[1005][1005] = { {0} };																//存储PT图邻接矩阵
bool expand_PT[2005][2005] = { {0} };														//存储扩图
bool tmp_PT[2005][2005] = { {0} };															//存储允许流
int pi[1005] = { 0 };
int tao[1005] = { 0 };
int n;																						//课程数量
int virtual_to_real[1005] = { 0 };															//重新编号，v’对应的v
int key_point[1005] = { 0 };																//所有的关键节点的虚拟节点号
bool mem[2005];																				//备忘录

int max(int x1, int x2)
{
	return (x1 > x2) ? x1 : x2;
}
void get_PT()																				//构造PT图
{
	cin >> n;
	int m, tmp;
	bool flag;
	for (int i = 1;i <= n;i++)
	{
		cin >> m;
		if (0 == m)																			//课程i没有前置课程
		{
			PT[0][i] = true;
		}
		else
		{
			for (int j = 0;j < m;j++)
			{
				cin >> tmp;
				PT[tmp][i] = true;
			}
		}
	}
	for (int i = 1;i <= n;i++)
	{
		flag = false;
		for (int j = 1;j <= n;j++)
		{
			if (PT[i][j])
			{
				flag = true;
				break;
			}
		}
		if (!flag) PT[i][n + 1] = true;
	}
}
void get_virtual()
{
	virtual_to_real[0] = 0, virtual_to_real[n + 1] = n + 1;
	bool mem[1005] = { 0 };																	//记录该点是否已编号
	for (int flag = 1;flag <= n;flag++)														//找第flag个节点
	{
		for (int i = 1;i <= n;i++)															//检查第i个点负度是否为0
		{
			if (mem[i]) continue;
			for (int j = 1;j <= n;j++)
			{
				if (PT[j][i] && !mem[j]) goto f1;											//i点负度非0，换点
			}
			virtual_to_real[flag] = i;
			mem[i] = true;
			break;
		f1:			continue;
		}
	}
}
void get_key_point()
{
	for (int i = 1;i <= n + 1;i++)
	{
		int& tmp = pi[i];
		for (int j = 0;j < i;j++)
		{
			if (!PT[virtual_to_real[j]][virtual_to_real[i]]) continue;
			tmp = max(pi[j] + 1, tmp);
		}
	}
	for (int i = n;i >= 0;i--)
	{
		int& tmp = tao[i];
		for (int j = n + 1;j > i;j--)
		{
			if (!PT[virtual_to_real[i]][virtual_to_real[j]]) continue;
			tmp = max(tao[j] + 1, tmp);
		}
	}
	cout << pi[n + 1] - 1 << ' ';
	int& k = pi[n + 1];
	key_point[0] = 0;
	for (int i = 1;i <= n;i++)
	{
		if (pi[i] + tao[i] == k) key_point[++key_point[0]] = i;
	}
}
int get_expand()
{
	//此函数中利用了参考博客的扩点思想，将原PT子图扩点，再求扩点后网络流的最大流
	int left = -1, right = 0;																//第i个点的左右子节点
	int m = key_point[0];
	key_point[0] = 0;
	for (int i = 1;i <= m;i++)
	{
		left += 2, right += 2;
		expand_PT[left][right] = true;
		for (int j = 0;j < i;j++)
		{
			if (PT[virtual_to_real[key_point[j]]][virtual_to_real[key_point[i]]] && pi[key_point[j]] + 1 == pi[key_point[i]]) expand_PT[2 * j][left] = true;
		}
	}
	for (int i = 1;i <= m;i++)
	{
		if (PT[virtual_to_real[key_point[i]]][n + 1]) expand_PT[2 * i][2 * m + 1] = true;
	}
	return m;
}
bool BFS(int m)
{
	for (int i = 0;i <= 2 * m + 1;i++) mem[i] = false;
	mem[0] = true;
	int head = 0, tail = 0;
	while (head <= tail)																				//对head进行检查
	{

		int& pos = queue[head].position;																//当前检查的节点
		for (int i = 1;i < pos;i++)
		{
			if (mem[i]) continue;
			if (!tmp_PT[i][pos]) continue;
			tail++;
			queue[tail].position = i;
			queue[tail].former = -1 * head;
			mem[i] = true;
		}
		for (int i = pos + 1;i <= 2 * m + 1;i++)
		{
			if (mem[i]) continue;
			if (!tmp_PT[pos][i] && expand_PT[pos][i])
			{
				tail++;
				queue[tail].position = i;
				queue[tail].former = head;
				mem[i] = true;
				if (i == 2 * m + 1)
				{
					int key = tail;
					while (key != 0)																		//增流
					{
						int& former = queue[key].former;
						if (former >= 0)
						{
							tmp_PT[queue[former].position][queue[key].position] = true;
							key = former;
						}
						else
						{
							tmp_PT[queue[key].position][queue[-1 * former].position] = false;
							key = -1 * former;
						}
					}
					return true;
				}
			}
		}
		head++;
	}
	return false;
}
int get_ans()
{
	int m = get_expand();																				//m表示网络流总点数
	queue[0].position = 0, queue[0].former = 0;
	while (BFS(m)) {}																						//找最大流
	int ans = 0;
	for (int i = 1;i <= 2 * m;i++)
	{
		if (tmp_PT[0][i]) ans++;
	}
	return ans;
}

int main()
{
	get_PT();
	get_virtual();
	get_key_point();
	cout << get_ans();
	return 0;
}

//声明：1.本题没有与其他同学做过任何交流
//		2.本次参考资料为CSDN博客：https://blog.csdn.net/u011346442/article/details/47658035 中保证每条路不相交的扩点思想
//确认：1.我独立完成了除了以上方面的全部内容，包括分析、设计、编码、调试、测试等。我清楚地知道，从以上方面获得的信息在一定程度上降低了作业难度，可能影响起评分。
//		2.我未使用他人的代码，不管是原封不动地复制，还是经过了某些等价转换。
//		3.我有义务妥善保管好我的代码，不会将其公开或给其他同学复制。
//		4.我编写的程序无意于破坏或妨碍任何计算机系统的正常运转。
//		5.如果违反了2、3、4条，对应的作业成绩将按0分计。