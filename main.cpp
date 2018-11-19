#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <set>

using namespace std;

double editDistance(const string& s1, const string& s2, double weight = 1.0);

set<string> func, var, condi;

vector<int> getPi(const string& s)
{
	const int size = (int)s.size();
	//	vector<int> pi(size, 0);
	vector<int> pi2(size, 0);
	for (int begin = 1; begin < size; ++begin)
	{
		for (int j = 0; j < size - begin; ++j)
		{
			if (s[begin + j] != s[j]) break;
			++pi2[begin + j];
			//			pi[begin + j] = max(pi[begin + j], j + 1);
		}
	}
	return pi2;
}

int aa = 0;
/*
N: 패턴
H: 원본
*/
vector<int> kmp(const string& origin, const string& pattern)
{
	const vector<int>& pi = getPi(pattern);
	vector<int> ret;
	const int ORI_SIZE = (int)origin.size(), PAT_SIZE = (int)pattern.size();
	int i, mat;
	i = mat = 0;
	while (i <= ORI_SIZE - PAT_SIZE)
	{
		if (origin[i + mat] == pattern[mat])
		{
			++mat;
			if (mat == PAT_SIZE)
			{
				ret.push_back(i);
				i += mat;
				mat = 0;
			}
		}
		else
		{
			if (mat == 0) ++i;
			else
			{
				i += mat - pi[mat - 1];
				mat = pi[mat - 1];
			}
		}
	}
	return ret;
}

vector<int> nomal(const string& origin, const string& pattern)
{
	for (int i = 0; i < origin.size() - pattern.size(); ++i)
	{
		int j;
		for (j = 0; j < pattern.size(); ++j)
		{
			if (origin[i + j] != pattern[j]) break;
		}
	}
	return {};
}

struct Code
{
public:
	const int SPACE = 32;
	const int TAB = 9; // '\t' 과 다르다. 

	const vector<string> condiPattern = { "if", "for", "while" }; // 조건 패턴
//	const vector<string> varPattern = { "bool", "char", "int", "struct", "class" };

	/*
	주석 및 연속 공백, 탭 제거
	원라인 코드로 변경
	*/
	string processCode;

	//	set<string> varName;
	//	set<string> funcName;
	vector<string> condiCode; // 조건 문자열 
//	int memUse;
//	int execTime;
//	string reCode;

	Code(const string& filePath) : processCode(" ")
	{
		FILE* f = fopen(filePath.c_str(), "rt");

		bool prevSpace = false;
		stack<char> com;

		char c, c2;
		while (true)
		{
			c = fgetc(f);
			c2 = 0;
			if (feof(f)) break;

			if (c == '/')
			{
				c2 = fgetc(f);
				if (feof(f)) break; // '/'등장 후 파일이 끝날일은 없다.

				if (c2 == '/') // 라인 주석 시작
				{
					if (!com.empty()) continue; // 이미 주석 공간이라면
					com.push('/');
					continue;
				}
				else if (c2 == '*') // 블럭 주석 시작
				{
					if (!com.empty()) continue; // 이미 주석 공간이라면
					com.push('*');
					continue;
				}
				else {}
			}
			else if (c == '*')
			{
				c2 = fgetc(f);
				if (feof(f)) break;

				if (c2 == '/') // 블럭 주석 끝
				{
					com.pop();
					continue;
				}
				else {}
			}
			else if (c == '\n')
			{
				if (!com.empty() && com.top() == '/') com.pop(); // 라인 주석 구간이라면 주석 구간이 끝난다.
				c = SPACE;
			}
			else if (c == TAB || c == '\t') c = SPACE; // 탭도 공백으로 치환 

			if (c == SPACE && prevSpace) continue; // 연속 SPACE 제거

			if (com.empty())
			{
				processCode.push_back(c);

				if (c == SPACE) prevSpace = true;
				else prevSpace = false;

				if (c2 == '\n' || c2 == TAB) c2 = SPACE;
				if (!prevSpace && c2 != 0) processCode.push_back(c2);

				if (c2 == SPACE) prevSpace = true;
				else if (c2 != 0) prevSpace = false;
			}
		}
		fclose(f);
	}

	void getCondiPattern()
	{
		for (auto pattern : condiPattern) // 검사할 pattern
		{
			const int size = (int)pattern.size();
			const vector<int>& idx = kmp(processCode, pattern);
			for (auto i : idx) // pattern 등장 인덱스
			{
				if (processCode[i - 1] != SPACE) continue; // pattern 시작 인덱스 전에 공백이 아니라면 그것은 패턴이 아니고 어떠한 문자열의 일부이다.

				int p = -1;
				// pattern 다음에 괄호가 나오지 않는 다는 것은 어떠한 문자열의 일부이다.
				if (processCode[i + size] == '(') p = i + size;
				else if (processCode[i + size] == ' ') p = i + size + 1;

				if (p == -1) continue;

				condiCode.emplace_back("");

				stack<char> s;
				s.push('(');
				while (!s.empty()) // 조건에서 함수 호출 때문에 괄호가 있을 수 있지만 삭제한다.
				{
					++p;
					if (processCode[p] == '(') s.push('(');
					else if (processCode[p] == ')') s.pop();
					else condiCode.back().push_back(processCode[p]);
				}
			}
		}
	}

	double condi_code(const Code& code, double weight = 1.0)
	{
		const int CONDI_SIZE_1 = (int)condiCode.size(), CONDI_SIZE_2 = (int)code.condiCode.size();
		double t = 0;
		for (int i = 0; i < CONDI_SIZE_1; ++i)
		{
			for (int j = 0; j < CONDI_SIZE_2; ++j)
			{
				t += editDistance(condiCode[i], code.condiCode[j]);
			}
		}
		if (CONDI_SIZE_1 * CONDI_SIZE_2 == 0) return 0;
		return (t / (CONDI_SIZE_1 * CONDI_SIZE_2)) * weight;
	}
};

double editDistance(const string& s1, const string& s2, double weight)
{
	const int SIZE_S1 = (int)s1.size(), SIZE_S2 = (int)s2.size();
	vector<int> d(SIZE_S2 + 1), d2(SIZE_S2 + 1);
	for (int i = 0; i <= SIZE_S2; ++i) d[i] = i;
	for (int i = 1; i <= SIZE_S1; ++i)
	{
		d2[0] = i - 1;
		for (int j = 1; j <= SIZE_S2; ++j)
		{
			d2[j] = min(d2[j - 1] + 1, min(d[j] + 1, d[j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1)));
		}
		d = d2;
	}
	if (max(SIZE_S1, SIZE_S2) == 0) return 1;
	return (1.0 - (double)d[SIZE_S2] / max(SIZE_S1, SIZE_S2)) * weight;
}

///////
using Cost = double;
const int DELTA_INF = 1.0;
int N;
vector<vector<Cost>> adj;

bool dfsMat(vector<int>& matY, vector<bool>& visited, int v)
{
	for (int i = 0; i < N; ++i)
	{
		if (adj[v][i] != 0) continue;

		if (visited[i]) continue;
		visited[i] = true;

		if (matY[i] == -1)
		{
			matY[i] = v;
			return true;
		}
		else
		{
			if (dfsMat(matY, visited, matY[i]))
			{
				matY[i] = v;
				return true;
			}
		}
	}
	return false;
}
int zero_edge_max_mat(vector<int>& matY)
{
	matY = vector<int>(N, -1);
	int mat = 0;
	for (int i = 0; i < N; ++i)
	{
		vector<bool> visited(N, false);
		if (dfsMat(matY, visited, i)) ++mat;
	}
	return mat;
}

ostream& operator<<(ostream& os, const vector<vector<Cost>>& matrix)
{
	const int N = (int)matrix.size();
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j) cout << matrix[i][j] << ' ';
		cout << '\n';
	}
	return os;
}

Cost confi(const vector<vector<Cost>>& matrix)
{
	::N = (int)matrix.size();
	::adj = matrix;

	for (int i = 0; i < N; ++i)
	{
		Cost t = adj[i][0];
		for (int j = 1; j < N; ++j) t = min<Cost>(t, adj[i][j]);
		for (int j = 0; j < N; ++j) adj[i][j] -= t;
	}
	for (int i = 0; i < N; ++i)
	{
		Cost t = adj[0][i];
		for (int j = 1; j < N; ++j) t = min<Cost>(t, adj[j][i]);
		for (int j = 0; j < N; ++j) adj[j][i] -= t;
	}

	vector<int> matY;
	while (zero_edge_max_mat(matY) != N)
	{
		vector<int> matX(N, -1);
		for (int i = 0; i < N; ++i) if (matY[i] != -1) matX[matY[i]] = i;

		vector<bool> visitedX(N, false), visitedY(N, false);
		queue<pair<int, bool>> q;
		for (int i = 0; i < N; ++i)
		{
			if (matX[i] == -1)
			{
				visitedX[i] = true;
				q.push({ i, true });
			}
		}

		while (!q.empty())
		{
			int v = q.front().first;
			bool X2Y = q.front().second; q.pop();

			if (X2Y) // X->Y
			{
				for (int i = 0; i < N; ++i)
				{
					if (adj[v][i] == 0 && !visitedY[i])
					{
						visitedY[i] = true;
						q.push({ i, false });
					}
				}
			}
			else // Y->X
			{
				for (int i = 0; i < N; ++i)
				{
					if (matY[v] == i && !visitedX[i])
					{
						visitedX[i] = true;
						q.push({ i, true });
						break;
					}
				}
			}
		}

		set<int> mvcX, mvcY; // minimum vertex cover
		for (int i = 0; i < N; ++i)
		{
			if (!visitedX[i]) mvcX.insert(i);
			if (visitedY[i]) mvcY.insert(i);
		}

		Cost delta = DELTA_INF;
		for (int i = 0; i < N; ++i)
			for (int j = 0; j < N; ++j)
				if (mvcX.find(i) == mvcX.end() && mvcY.find(j) == mvcY.end())
					delta = min<Cost>(delta, adj[i][j]);

		for (int i = 0; i < N; ++i)
		{
			bool iV = mvcX.find(i) != mvcX.end();
			for (int j = 0; j < N; ++j)
			{
				bool jV = mvcY.find(j) != mvcY.end();

				if (iV && jV) adj[i][j] += delta;
				else if (!iV && !jV) adj[i][j] -= delta;
			}
		}
	}

	Cost ret = 0;
	for (int i = 0; i < N; ++i) ret += matrix[matY[i]][i];
	return ret;
}

Cost confi(const Code& code1, const Code& code2)
{
	const int XN = (int)code1.condiCode.size(), YN = (int)code2.condiCode.size();
	if (XN < YN) return confi(code2, code1);
	if (XN == 0) return 1;

	vector<vector<Cost>> matrix = vector<vector<Cost>>(XN, vector<Cost>(XN, 0));

	for (int i = 0; i < XN; ++i)
		for (int j = 0; j < YN; ++j) matrix[i][j] = -editDistance(code1.condiCode[i], code2.condiCode[j]);
	return confi(matrix) * -1 / XN;
}
///////

void fileCondi(const string& path, const vector<string>& users)
{
	const int N = (int)users.size();
	vector<vector<Cost>> ret(N, vector<Cost>(N, -1));
	vector<Code> codes;
	for (int i = 0; i < N; ++i)
	{
		codes.emplace_back(path + users[i]);
		codes.back().getCondiPattern();
	}
	for (int i = 0; i < N; ++i)
	{
		for (int j = i + 1; j < N; ++j)
		{
			const Cost patternSim = confi(codes[i], codes[j]);
			const Cost code_edit_dis = editDistance(codes[i].processCode, codes[j].processCode);
			
			Cost cost = int((patternSim * 0.4 + code_edit_dis * 0.6) * 10000) / 100.0;
			
			ret[i][j] = ret[j][i] = cost;
		}
	}

	FILE* csv = fopen((path + "result.csv").c_str(), "wt");
	for (int i = 0; i < N; ++i) fprintf(csv, ",%s", users[i].c_str());
	fputc('\n', csv);
	for (int i = 0; i < N; ++i)
	{
		fprintf(csv, "%s,", users[i].c_str());
		for (int j = 0; j < N; ++j) fprintf(csv, "%.2lf,", ret[i][j]);
		fputc('\n', csv);
	}
	fclose(csv);
}


#include <chrono>

int main()
{
	Code code1("test1.cpp"), code2("test2.cpp");

	code1.getCondiPattern();
	code2.getCondiPattern();

	//////////////////////////////////////////////////////////////////////////
	cout << "<code1의 가공 코드>" << endl;
	cout << code1.processCode << endl;

	cout << endl << "<code2의 가공 코드>" << endl;
	cout << code2.processCode << endl;
	cout << endl;

	//////////////////////////////////////////////////////////////////////////
	cout << "<code1의 조건 패턴>" << endl;
	for (auto i : code1.condiCode) cout << i << endl;

	cout << endl << "<code2의 조건 패턴>" << endl;
	for (auto i : code2.condiCode) cout << i << endl;
	cout << endl;

	//////////////////////////////////////////////////////////////////////////
	cout << "<각 영역 유사도>" << endl;
	const double patternSim = confi(code1, code2);
	cout << "조건 패턴 유사도: " << patternSim << endl;
	const double code_edit_dis = editDistance(code1.processCode, code2.processCode);
	cout << "전체 코드 편집 거리: " << code_edit_dis << endl;
	cout << endl;

	//////////////////////////////////////////////////////////////////////////
	const Cost PATTERN_WEIGHT = 0.4;
	const Cost CODE_EDIT_WEIGHT = 0.6;
	cout << "<각 영역 유사도 가중치>" << endl;
	cout << "조건: " << patternSim << " * " << PATTERN_WEIGHT <<  " = " << patternSim * PATTERN_WEIGHT << endl;
	cout << "편집 거리: " << code_edit_dis << " * " << CODE_EDIT_WEIGHT << " = " << code_edit_dis * CODE_EDIT_WEIGHT << endl;
	cout << endl;

	//////////////////////////////////////////////////////////////////////////
	cout << "<최종>" << endl;
	cout << "유사도: " << (patternSim * PATTERN_WEIGHT + code_edit_dis * CODE_EDIT_WEIGHT) * 100 << "%" << endl;

	//////////////////////////////////////////////////////////////////////////
	

	return 0;
}