#include <iostream>
#include <iomanip>
#include<string>
using namespace std;
#include "CodeGenerator.h"


static int Stack_Address = 5;
const int MAX = 100;

typedef struct breakLabelList {
	string lab;
	breakLabelList* next = NULL;
}breakLabelList;


static breakLabelList* breakList = NULL;
static int switchCaseNum = 0;
static int switchCount = 0;


void addStack(double d)
{
	printf("ldc %f\n", d);
}

int isConstant(treenode *root);
double calculateValue(treenode *root);

double calculateValue(treenode* root)
{
	leafnode* leaf;
	if_node* ifn;
	int x1, x2;

	if ((root->hdr.which == LEAF_T))
	{
		leaf = (leafnode*)root;

		if (leaf->hdr.type == TN_INT)
		{
			return leaf->data.ival;
		}
		else if (leaf->hdr.type == TN_REAL)
		{
			return leaf->data.dval;
		}
		else
		{
			return 0;
		}
	}

	if ((root->hdr.which == IF_T))
	{
		if (root->hdr.type == TN_COND_EXPR)
		{
			ifn = (if_node*)root;
			if (calculateValue(ifn->cond)) {
				return (calculateValue(ifn->then_n));
			}
			else {
				return (calculateValue(ifn->else_n));
			}
		}
	}

	switch (root->hdr.tok)
	{
	case PLUS:
		return (calculateValue(root->lnode) + calculateValue(root->rnode));
		break;

	case MINUS:
		if (root->lnode == NULL)
		{
			return (-calculateValue(root->lnode));
		}
		return (calculateValue(root->lnode) - calculateValue(root->rnode));
		break;
	case DIV:
		//diff
		x1 = isConstant(root->lnode);
		x2 = isConstant(root->rnode);
		if (x1 && x2)
		{
			return (calculateValue(root->lnode) * calculateValue(root->rnode));
		}
		else if (x1)
		{
			if (calculateValue(root->lnode) == 0)
			{
				return 0;
			}
		}
		break;

	case STAR:
		//diff
		x1 = isConstant(root->lnode);
		x2 = isConstant(root->rnode);
		if (x1 && x2)
		{
			return (calculateValue(root->lnode) * calculateValue(root->rnode));
		}
		else if (x1)
		{
			if (calculateValue(root->lnode) == 0)
			{
				return 0;
			}
		}
		else if (x2)
		{
			if (calculateValue(root->rnode) == 0)
			{
				return 0;
			}
		}

		break;

	case AND:
		//diff
		x1 = isConstant(root->lnode);
		x2 = isConstant(root->rnode);
		if (x1 && x2)
		{
			return (calculateValue(root->lnode) * calculateValue(root->rnode));
		}
		else if (x1 || x2)
		{
			return 0;
		}
		break;

	case OR:
		//diff
		x1 = isConstant(root->lnode);
		x2 = isConstant(root->rnode);
		if (x1 && x2)
		{
			return (calculateValue(root->lnode) * calculateValue(root->rnode));
		}
		else if (x1 || x2)
		{
			return 1;
		}
		break;

	case NOT:
		return (!calculateValue(root->rnode));
		break;

	case GRTR:
		/* Greater token ">" */
		return (calculateValue(root->lnode) > calculateValue(root->rnode));
		break;

	case LESS:
		/* Less token "<" */
		return (calculateValue(root->lnode) < calculateValue(root->rnode));
		break;

	case EQUAL:
		/* Equal token "==" */
		return (calculateValue(root->lnode) == calculateValue(root->rnode));
		break;

	case NOT_EQ:
		/* Not equal token "!=" */
		return (calculateValue(root->lnode) != calculateValue(root->rnode));
		break;

	case LESS_EQ:
		/* Less or equal token "<=" */
		return (calculateValue(root->lnode) <= calculateValue(root->rnode));
		break;

	case GRTR_EQ:
		/* Greater or equal token ">=" */
		return (calculateValue(root->lnode) >= calculateValue(root->rnode));
		break;

	default:
		return 0;
	}
	return 0;
}

int isConstant(treenode* root)
{

	leafnode* leaf;
	if_node* ifn;
	int x1, x2;

	if ((root->hdr.which == LEAF_T))
	{
		leaf = (leafnode*)root;
		if (leaf->hdr.type == TN_INT || leaf->hdr.type == TN_REAL)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	if ((root->hdr.which == IF_T))
	{
		if (root->hdr.type == TN_COND_EXPR)
		{
			ifn = (if_node*)root;
			if (isConstant(ifn->cond)) {
				if (calculateValue(ifn->cond)) {
					return (isConstant(ifn->then_n));
				}
				else {
					return (isConstant(ifn->else_n));
				}
			}
		}
	}

	switch (root->hdr.tok)
	{
	case PLUS:
		return (isConstant(root->lnode) && isConstant(root->rnode));
		break;

	case MINUS:
		/* Minus token "-" */
		if (root->lnode == NULL)
		{
			return (isConstant(root->lnode));
		}
		return (isConstant(root->lnode) && isConstant(root->rnode));
		break;
	case DIV:
		x1 = isConstant(root->lnode);
		x2 = isConstant(root->rnode);
		if (x1 && x2)
		{
			return 1;
		}
		else if (x1)
		{
			x1 = calculateValue(root->lnode);
			return x1 == 0;
		}
		else
		{
			return 0;
		}
		break;

	case STAR:
		//diff
		x1 = isConstant(root->lnode);
		x2 = isConstant(root->rnode);
		if (x1 && x2)
		{
			return 1;
		}
		else if (x1)
		{
			x1 = calculateValue(root->lnode);
			return x1 == 0;
		}
		else if (x2)
		{
			x2 = calculateValue(root->rnode);
			return x2 == 0;
		}
		else
		{
			return 0;
		}
		break;

	case AND:
		//diff
		x1 = isConstant(root->lnode);
		x2 = isConstant(root->rnode);
		if (x1 && x2)
		{
			return 1;
		}
		else if (x1)
		{
			x1 = calculateValue(root->lnode);
			return !x1;
		}
		else if (x2)
		{
			x2 = calculateValue(root->rnode);
			return !x2;
		}
		else
		{
			return 0;
		}
		break;

	case OR:
		//diff
		x1 = isConstant(root->lnode);
		x2 = isConstant(root->rnode);
		if (x1 && x2)
		{
			return 1;
		}
		else if (x1)
		{
			x1 = calculateValue(root->lnode);
			return x1;
		}
		else if (x2)
		{
			x2 = calculateValue(root->rnode);
			return x2;
		}
		else
		{
			return 0;
		}
		break;

	case NOT:
		return (isConstant(root->rnode));
		break;

	case GRTR:
		/* Greater token ">" */
		return (isConstant(root->lnode) && isConstant(root->rnode));
		break;

	case LESS:
		/* Less token "<" */
		return (isConstant(root->lnode) && isConstant(root->rnode));
		break;

	case EQUAL:
		/* Equal token "==" */
		return (isConstant(root->lnode) && isConstant(root->rnode));
		break;

	case NOT_EQ:
		/* Not equal token "!=" */
		return (isConstant(root->lnode) && isConstant(root->rnode));
		break;

	case LESS_EQ:
		/* Less or equal token "<=" */
		return (isConstant(root->lnode) && isConstant(root->rnode));
		break;

	case GRTR_EQ:
		/* Greater or equal token ">=" */
		return (isConstant(root->lnode) && isConstant(root->rnode));
		break;

	default:
		return 0;
	}
}




struct list
{
	int s;
	struct list *next;
};

//Variable1 struct
typedef struct Variable1
{
	char *name;
	//0 = int, 1 = double, 2 = float
	char *type;
	int address;
	int size;

	int pointerSize;
	struct Variable1 *next;

	//for arrays
	struct list *arrSizes;

} Variable1;

typedef struct StructDef
{

	char *name;
	int size;

	Variable1 *Variable1List;

	struct StructDef *next;
} structdef;

typedef struct tableStruct
{
	int nextAddress;
	Variable1 *list;

} tableStruct;

//label counter
int LAB = 1;
//where break should go
int breakLabel = 1;
//if Variable1 should add address or value
int addAddress = 0;
//symbol table
tableStruct *s;
//list for sizes of the array
struct list *sizesOfArr;
//Variable chosen
Variable1 *var = NULL;
//structList
structdef *structList = NULL;
//truct using
structdef *struc = NULL;

//switch constant
int isConstSwitch;
int switchValue;
int printBreak = 1;

class Variable
{

	/* Think! what does a Variable contain? */
	string identifier, type;
	int address, size;
	Variable* next;

public:
	Variable()
	{
		next = NULL;
	}

	Variable(string key, string type, int address, int size)
	{
		this->identifier = key;
		this->size = size;
		this->type = type;
		this->address = address;
		next = NULL;
	}
	friend class SymbolTable;
};

class SymbolTable
{
	/* Think! what can you add to  symbol_table */
	Variable* head[MAX];

public:

	SymbolTable()
	{
		for (int i = 0; i < MAX; i++)
			head[i] = NULL;
	}

	// Function to find an identifier 
	int find(string id)
	{
		int index = hashf(id);
		Variable* start = head[index];

		if (start == NULL)
			return -1;

		while (start != NULL)
		{

			if (start->identifier == id)
			{
				return start->address;
			}

			start = start->next;
		}

		return -1; // not found 
	}

	// Function to insert an identifier 
	bool insert(string id, string type, int address, int size)
	{
		int index = hashf(id);
		Variable* p = new Variable(id, type, address, size);

		if (head[index] == NULL)
		{
			head[index] = p;
			return true;
		}

		else
		{
			Variable* start = head[index];
			while (start->next != NULL)
				start = start->next;
			start->next = p;
			return true;
		}

		return false;
	}

	int hashf(string id)
	{
		int asciiSum = 0;

		for (int i = 0; i < id.length(); i++)
		{
			asciiSum = asciiSum + id[i];
		}

		return (asciiSum % MAX);
	}

};
SymbolTable ST;


class TreeNode { //base class
public:
	/*you can add another son nodes */
	TreeNode* son1 = NULL;
	TreeNode* son2 = NULL;
	TreeNode* son3 = NULL;
	TreeNode* son4 = NULL;
	virtual ~TreeNode() {};
	TreeNode() {};
	virtual void gencode(string c_type = "coder")
	{
		if (son1 != NULL) son1->gencode(c_type);
		if (son2 != NULL) son2->gencode(c_type);
		if (son3 != NULL) son3->gencode(c_type);
		if (son4 != NULL) son4->gencode(c_type);
	};
};
/*
* you have to add functions/implement of gencode()... of derived classes
*/

class TypeClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
	}
};

class FunctionDefClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		if (son3 != NULL) son3->gencode("coder");
		if (son4 != NULL) son4->gencode("coder");
	}
};

class StatmentClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
	}
};

class StatmentListClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
	}
};


class DeclClass :public TreeNode
{
public:
	string name;
	string type;
	DeclClass(string name, string type)
	{
		this->name = name;
		this->type = type;
	}
	virtual void gencode(string c_type)
	{
		ST.insert(name, type, Stack_Address++, 1);
	}
	friend SymbolTable;
};

class Assign :public TreeNode
{
public:
	virtual void gencode(string c_type) {
		if (son1 != NULL) son1->gencode("codel"); //return address 
		if (son2 != NULL) son2->gencode("coder"); //return value
		cout << "sto" << endl;
	}
};

class DeclListClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
	}
};

//get address then value then constant value
class PlusEqClass :public TreeNode
{
public:
	virtual void gencode(string c_type) {
		if (son1 != NULL) son1->gencode("codel"); //return address
		if (son1 != NULL) son1->gencode("coder"); //return address 
		if (son2 != NULL) son2->gencode("coder"); // return value
		cout << "add" << endl;
		cout << "sto" << endl;
	}
};

class MinusEqClass :public TreeNode
{
public:
	virtual void gencode(string c_type) {
		if (son1 != NULL) son1->gencode("codel"); //return address
		if (son1 != NULL) son1->gencode("coder"); //return address 
		if (son2 != NULL) son2->gencode("coder"); // return value
		cout << "sub" << endl;
		cout << "sto" << endl;
	}
};

class MulEqClass :public TreeNode
{
public:
	virtual void gencode(string c_type) {
		if (son1 != NULL) son1->gencode("codel"); //return address
		if (son1 != NULL) son1->gencode("coder"); //return address 
		if (son2 != NULL) son2->gencode("coder"); // return value
		cout << "mul" << endl;
		cout << "sto" << endl;
	}
};

class DivEqClass :public TreeNode
{
public:
	virtual void gencode(string c_type) {
		if (son1 != NULL) son1->gencode("codel"); //return address
		if (son1 != NULL) son1->gencode("coder"); //return address 
		if (son2 != NULL) son2->gencode("coder"); // return value
		cout << "div" << endl;
		cout << "sto" << endl;
	}
};

class Id :public TreeNode
{
public:
	string id_name;
	Id(string id_n) {
		id_name = id_n;
	}
	virtual void gencode(string c_type)
	{
		cout << "ldc " << ST.find(id_name) << endl;

		if (c_type == "coder" || c_type == "arrcoder")
		{
			cout << "ind " << endl;
		}
	}
};

class IntNum :public TreeNode
{
public:
	int value;
	IntNum(int number) { value = number; }
	virtual void gencode(string c_type) {
		cout << "ldc " << value << endl;
	}
};

//like int class but double
class RealNum :public TreeNode
{
public:
	double value;
	double getValue() {
		return this->value;
	}
	RealNum(double _number) { value = _number; }
	virtual void gencode(string c_type) {
		cout << "ldc " << fixed << setprecision(2) << getValue() << endl;
	}
};

class BreakClass : public TreeNode
{
public:
	void gencode(string c_type)
	{
		if (breakList != NULL) {
			cout << "ujp " << breakList->lab << " " << endl;
		}
		if (son1 != NULL) son1->gencode();
		if (son2 != NULL) son2->gencode();

	}

};

class LabelClass : public TreeNode {

public:
	void gencode(string c_type)
	{
		if (c_type == "cases_exp")
		{
			cout << "switch_" << switchCount << "_c" << switchCaseNum << ": " << endl;
			if (son2 != NULL) son2->gencode();
			cout << "ujp end_switch_" << switchCount << " " << endl;
			switchCaseNum++;
		}
		else if (c_type == "cases_val")
		{
			if (son1 != NULL) son1->gencode();
		}
		else {
			if (son1 != NULL) son1->gencode();
			if (son2 != NULL) son2->gencode();

		}
	}

};

class SwitchClass : public TreeNode
{
public:
	void gencode(string c_type = "coder")
	{
		switchCaseNum = 0;
		int lab = switchCount;
		cout << "ujp switch_" << lab << " " << endl;
		son2->gencode("cases_exp");
		cout << "switch_" << lab << ": " << endl;
		son2->gencode("cases_val");
		for (int i = switchCaseNum - 1; i >= 0; i--)
		{
			son1->gencode();
			cout << "neq " << endl;
			cout << "fjp switch_" << lab << "_c" << i << endl;
		}
		cout << "end_switch_" << lab << ": " << endl;
		switchCount++;
	}
};

class SubClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 == NULL && son2 != NULL)
		{ //negative value and not Submission
			if (son2 != NULL) son2->gencode("coder");
			cout << "neg" << endl;
		}
		else
		{
			if (son1 != NULL) son1->gencode("coder");
			if (son2 != NULL) son2->gencode("coder");
			cout << "sub" << endl;
		}
	}
};

class PlusClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		cout << "add" << endl;
	}
};

class MulClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		cout << "mul" << endl;
	}
};

class DivClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		cout << "div" << endl;
	}
};

// ==
class EquClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		cout << "equ" << endl;
	}
};

// !=
class NEquClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		cout << "neq" << endl;
	}
};

// <
class LessClass :public TreeNode
{ //Going left and right to gencode other objects and printing les at the end
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		cout << "les" << endl;
	}
};

// >
class GreaterClass :public TreeNode
{ //Going left and right to gencode other objects and printing grt at the end
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		cout << "grt" << endl;
	}
};

// >=
class GreaterEquClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		cout << "geq" << endl;
	}
};

// <=
class LessEquClass :public TreeNode
{ //Going left and right to gencode other objects and printing leq at the end
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		cout << "leq" << endl;
	}
};

// x++
class IncClass :public TreeNode
{ //Going left and right to gencode other objects and printing inc 1 at the end
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) //This is the case i++
		{
			son1->gencode("coder");
			son1->gencode("codel");
			son1->gencode("coder");
			cout << "inc" << endl;
			cout << "sto" << endl;
		}
		if (son2 != NULL)
		{
			son2->gencode("codel");
			son2->gencode("coder");
			cout << "inc" << endl;
			cout << "sto" << endl;
			son2->gencode("coder");
		}
	}
};

// x--
class DecClass :public TreeNode
{ //Going left and right to gencode other objects and printing dec 1 at the end
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL)
		{
			son1->gencode("coder");
			son1->gencode("codel");
			son1->gencode("coder");
			cout << "dec" << endl;
			cout << "sto" << endl;
		}
		if (son2 != NULL)
		{
			son2->gencode("codel");
			son2->gencode("coder");
			cout << "dec" << endl;
			cout << "sto" << endl;
			son2->gencode("coder");
		}
	}
};

// &&
class AndClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		cout << "and" << endl;
	}
};

// ||
class OrClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		cout << "or" << endl;
	}
};

// !
class NotClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
		cout << "not" << endl;
	}
};

// print
class FuncCallClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son2 != NULL) son2->gencode("coder");
		cout << "print" << endl;
	}
};

static int count1 = 0;

class IfClass :public TreeNode
{ //If-No-Else class
public:
	int theCounter;
	IfClass()
	{
		theCounter = count1++;
	}
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		cout << "fjp if_end" << theCounter << endl;
		if (son2 != NULL) son2->gencode("coder");
		cout << "if_end" << theCounter << ":" << endl;
	}
};

static int count2 = 0;

class IfAndElseClass :public TreeNode
{ //If-Then-Else class
public:
	int theCounter;
	IfAndElseClass()
	{
		theCounter = count2++;
	}
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		cout << "fjp ifelse_else" << theCounter << endl;
		if (son2 != NULL) son2->gencode("coder");
		cout << "ujp ifelse_end" << theCounter << endl;
		cout << "ifelse_else" << theCounter << ":" << endl;
		if (son3 != NULL) son3->gencode("coder");
		cout << "ifelse_end" << theCounter << ":" << endl;
	}
};


static int count3 = 0;

class CondIfClass :public TreeNode
{
public:
	int theCounter;
	CondIfClass()
	{
		theCounter = count3++;
	}
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		cout << "fjp condition_else" << theCounter << endl;
		if (son2 != NULL) son2->gencode("coder");
		cout << "ujp condition_end" << theCounter << endl;
		cout << "condition_else" << theCounter << ":" << endl;
		if (son3 != NULL) son3->gencode("coder");
		cout << "condition_end" << theCounter << ":" << endl;
	}
};

static int count4 = 0;

class ForClass :public TreeNode
{
public:
	int theCounter;
	ForClass()
	{
		theCounter = count4++;
	}
	virtual void gencode(string c_type)
	{
		breakLabelList* BRL = new breakLabelList;
		BRL->lab = "for_end" + to_string(theCounter);
		BRL->next = breakList;
		breakList = BRL;

		if (son1 != NULL) son1->gencode("coder");
		cout << "for_loop" << theCounter << ":" << endl;
		if (son2 != NULL) son2->gencode("coder");
		cout << "fjp for_end" << theCounter << endl;
		if (son3 != NULL) son3->gencode("coder");
		if (son4 != NULL) son4->gencode("coder");
		cout << "ujp for_loop" << theCounter << endl;
		cout << "for_end" << theCounter << ":" << endl;

		breakList = BRL->next;
		delete BRL;
	}
};


static int count5 = 0;

class WhileClass :public TreeNode
{
public:
	int theCounter;
	WhileClass()
	{
		theCounter = count5++;
	}
	virtual void gencode(string c_type)
	{
		breakLabelList* BRL = new breakLabelList;
		BRL->lab = "while_end" + to_string(theCounter);
		BRL->next = breakList;
		breakList = BRL;

		cout << "while_loop" << theCounter << ":" << endl;
		if (son1 != NULL) son1->gencode("coder");
		cout << "fjp while_end" << theCounter << endl;
		if (son2 != NULL) son2->gencode("coder");
		cout << "ujp while_loop" << theCounter << endl;
		cout << "while_end" << theCounter << ":" << endl;

		breakList = BRL->next;
		delete BRL;
	}
};

class TypeListClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
	}
};


static int count6 = 0;

class DoWhileClass :public TreeNode
{
public:
	int theCounter;
	DoWhileClass()
	{
		theCounter = count6++;
	}
	virtual void gencode(string c_type)
	{
		cout << "do_while_loop" << theCounter << ":" << endl;
		if (son2 != NULL) son2->gencode("coder");
		if (son1 != NULL) son1->gencode("coder");
		cout << "fjp do_while_end" << theCounter << endl;
		cout << "ujp do_while_loop" << theCounter << endl;
		cout << "do_while_end" << theCounter << ":" << endl;
	}
};

class FunDeclerationClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
	}
};

class BlockClass :public TreeNode
{
public:
	virtual void gencode(string c_type)
	{
		if (son1 != NULL) son1->gencode("coder");
		if (son2 != NULL) son2->gencode("coder");
	}
};

int obj_tree(treenode* root);

void addsizeTable(int x)
{
	struct list *l = (struct list *)malloc(sizeof(struct list));
	l->s = x;
	l->next = NULL;

	if (var->arrSizes == NULL)
	{
		var->arrSizes = l;
	}
	else
	{

		l->next = var->arrSizes;
		var->arrSizes = l;
	}
}

Variable1 *findVar(char *name)
{
	Variable1 *v = s->list;
	while (v != NULL)
	{
		if (strcmp(name, v->name) == 0)
		{
			return v;
		}
		v = v->next;
	}
	return NULL;
}

Variable1 *findVarInStruct(char *name, structdef *str)
{
	Variable1 *v = str->Variable1List;
	while (v != NULL)
	{
		if (strcmp(name, v->name) == 0)
		{
			return v;
		}
		v = v->next;
	}
	return NULL;
}

structdef *findVarStruct(char *name)
{
	structdef *v = structList;
	while (v != NULL)
	{
		if (strcmp(name, v->name) == 0)
		{
			return v;
		}
		v = v->next;
	}
	return NULL;
}

int sizeofVar(Variable1 *of)
{
	if (of->pointerSize >= 1)
	{
		return 1;
	}

	structdef *v = findVarStruct(of->type);
	if (v != NULL)
	{
		return v->size;
	}

	return 1;
}

void add(Variable1 *v, tableStruct *st)
{
	v->next = st->list;
	st->list = v;
	v->address = st->nextAddress;
	(st->nextAddress) += (v->size) * sizeofVar(v);
}

structdef *newStruct(char *name)
{
	structdef *s = (structdef *)malloc(sizeof(structdef));
	s->name = name;
	s->next = NULL;
	s->Variable1List = NULL;
	s->size = 0;

	if (structList != NULL)
	{
		s->next = structList;
	}

	structList = s;

	return s;
}

void addVariable1ToStruct(Variable1 *v)
{

	if (struc->Variable1List != NULL)
	{
		v->next = struc->Variable1List;
	}
	struc->Variable1List = v;

	v->address = struc->size;

	(struc->size) += (v->size) * (sizeofVar(v));
}

tableStruct *newSymbolTable(tableStruct *s)
{
	if (s == NULL)
	{
		tableStruct *ns = (tableStruct *)malloc(sizeof(tableStruct));
		ns->list = NULL;
		ns->nextAddress = 5;
		return ns;
	}
	return s;
}



int code_recur(treenode* root)
{
	int printedTree;
	printedTree = obj_tree(root);
	return SUCCESS;
}

int obj_tree(treenode *root)
{

	s = newSymbolTable(s);
	if_node *ifn;
	for_node *forn;
	leafnode *leaf;
	treenode *tn;

	Variable1 *v;
	struct list *list_;
	int la, lb, left;
	int prevBreak;

	if (!root)
		return SUCCESS;

	switch (root->hdr.which)
	{
	case LEAF_T:
		leaf = (leafnode *)root;
		switch (leaf->hdr.type)
		{
		case TN_LABEL:
			/* Maybe you will use it later */
			break;

		case TN_IDENT:
			var = findVar(leaf->data.sval->str);
			printf("ldc %d\n", var->address);
			if (!addAddress)
			{
				printf("ind\n");
			}

			break;

		case TN_COMMENT:
			/* Maybe you will use it later */
			break;

		case TN_ELLIPSIS:
			/* Maybe you will use it later */
			break;

		case TN_STRING:
			/* Maybe you will use it later */
			break;

		case TN_TYPE:
			/* Maybe you will use it later */
			break;

		case TN_INT:
			printf("ldc %d\n", leaf->data.ival);
			break;

		case TN_REAL:
			printf("ldc %f\n", leaf->data.dval);
			break;
		}
		break;

	case IF_T:
		ifn = (if_node *)root;
		switch (ifn->hdr.type)
		{

		case TN_IF:
			if (ifn->else_n == NULL)
			{
				/* if case (without else)*/
				if(isConstant(ifn->cond)){
					if(calculateValue(ifn->cond)){
						obj_tree(ifn->then_n);
					}
					break;
				}
				int la = LAB++;
				obj_tree(ifn->cond);
				printf("fjp label%d\n", la);
				obj_tree(ifn->then_n);
				printf("label%d:\n", la);
			}
			else
			{
				/* if - else case*/
				if(isConstant(ifn->cond)){
					if(calculateValue(ifn->cond)){
						obj_tree(ifn->then_n);
					}else{
						obj_tree(ifn->else_n);
					}
					break;
				}
				int la = LAB++;
				int lb = LAB++;
				obj_tree(ifn->cond);
				printf("fjp label%d\n", la);
				obj_tree(ifn->then_n);
				printf("ujp label%d\n", lb);
				printf("label%d:\n", la);
				obj_tree(ifn->else_n);
				printf("label%d:\n", lb);
			}
			break;

		case TN_COND_EXPR:
			/* (cond)?(exp):(exp); */
			if(isConstant(ifn->cond)){
				if(calculateValue(ifn->cond)){
					obj_tree(ifn->then_n);
				}else{
					obj_tree(ifn->else_n);
				}
				break;
			}
			
			la = LAB++;
			lb = LAB++;
			obj_tree(ifn->cond);
			printf("fjp label%d\n", la);
			obj_tree(ifn->then_n);
			printf("ujp label%d\n", lb);
			printf("label%d:\n", la);
			obj_tree(ifn->else_n);
			printf("label%d:\n", lb);
			break;

		default:
			/* Maybe you will use it later */
			obj_tree(ifn->cond);
			obj_tree(ifn->then_n);
			obj_tree(ifn->else_n);
		}
		break;

	case FOR_T:
		forn = (for_node *)root;
		switch (forn->hdr.type)
		{

		case TN_FUNC_DEF:
			/* Function definition */
			/* e.g. int main(...) { ... } */
			/* Look at the output AST structure! */

			//obj_tree(forn->init);
			obj_tree(forn->test->rnode);
			//obj_tree(forn->incr);
			obj_tree(forn->stemnt);

			break;

		case TN_FOR:
			/* For case*/
			/* e.g. for(i=0;i<5;i++) { ... } */
			/* Look at the output AST structure! */
			la = LAB++;
			lb = LAB++;
			prevBreak = breakLabel;
			breakLabel = la;
			obj_tree(forn->init);
			printf("label%d:\n", lb);
			obj_tree(forn->test);
			printf("fjp label%d\n", la);
			obj_tree(forn->stemnt);
			obj_tree(forn->incr);
			printf("ujp label%d\n", lb);
			printf("label%d:\n", la);
			breakLabel = prevBreak;
			break;

		default:
			/* Maybe you will use it later */
			obj_tree(forn->init);
			obj_tree(forn->test);
			obj_tree(forn->stemnt);
			obj_tree(forn->incr);
		}
		break;

	case NODE_T:
		switch (root->hdr.type)
		{
		case TN_PARBLOCK:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_PARBLOCK_EMPTY:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_TRANS_LIST:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_FUNC_DECL:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_FUNC_CALL:
			/* Function call */
			if (strcmp(((leafnode *)root->lnode)->data.sval->str, "printf") == 0)
			{
				/* printf case */
				/* The expression that you need to print is located in */
				/* the currentNode->right->right sub tree */
				/* Look at the output AST structure! */

				obj_tree(root->rnode->rnode);
				printf("print\n");
			}
			else
			{
				/* other function calls - for HW3 */
				obj_tree(root->lnode);
				obj_tree(root->rnode);
			}
			break;

		case TN_BLOCK:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_ARRAY_DECL:
			/* array declaration - for HW2 */

			addsizeTable(((leafnode *)root->rnode)->data.ival);
			la = 1;
			if (root->lnode->hdr.type == TN_ARRAY_DECL)
			{
				la = obj_tree(root->lnode);
			}
			else
			{
				var->name = ((leafnode *)root->lnode)->data.sval->str;
			}

			return ((leafnode *)root->rnode)->data.ival * la;
			break;

		case TN_EXPR_LIST:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_NAME_LIST:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_ENUM_LIST:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_FIELD_LIST:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_PARAM_LIST:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_IDENT_LIST:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_TYPE_LIST:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_COMP_DECL:
			/* struct component declaration - for HW2 */
			v = (Variable1 *)malloc(sizeof(Variable1));

			var = v;
			if (root->lnode->lnode->hdr.type == TN_OBJ_REF)
			{
				v->type = ((leafnode *)root->lnode->lnode->lnode)->data.sval->str;
			}
			else
			{
				v->type = toksym(((leafnode *)root->lnode->lnode)->hdr.tok, 0);
			}

			v->arrSizes = NULL;

			if (root->rnode->lnode->hdr.type == TN_PNTR)
			{
				v->pointerSize = obj_tree(root->rnode->lnode);
				tn = root->rnode->rnode;
			}
			else
			{
				v->pointerSize = 0;
				tn = root->rnode;
			}

			if (tn->hdr.type == TN_ARRAY_DECL)
			{
				var = v;
				v->size = obj_tree(tn);
			}
			else
			{
				v->size = 1;
				v->name = ((leafnode *)tn)->data.sval->str;
			}

			addVariable1ToStruct(v);
			break;

		case TN_DECL:
			//right = id
			//right -> keft = pntr -> pointer type

			if (root->lnode->lnode->hdr.type == TN_OBJ_DEF)
			{
				obj_tree(root->lnode->lnode);
				break;
			}

			v = (Variable1 *)malloc(sizeof(Variable1));
			var = v;
			if (root->lnode->lnode->hdr.type == TN_OBJ_REF)
			{
				v->type = ((leafnode *)root->lnode->lnode->lnode)->data.sval->str;
			}
			else
			{
				v->type = toksym(((leafnode *)root->lnode->lnode)->hdr.tok, 0);
			}

			v->size = 1;
			v->arrSizes = NULL;

			if (root->rnode->lnode->hdr.type == TN_PNTR)
			{
				v->pointerSize = obj_tree(root->rnode->lnode);
				tn = root->rnode->rnode;
			}
			else
			{
				v->pointerSize = 0;
				tn = root->rnode;
			}

			if (tn->hdr.type == TN_ARRAY_DECL)
			{
				var = v;
				v->size = obj_tree(tn);
			}
			else
			{
				v->name = ((leafnode *)tn)->data.sval->str;
			}

			add(v, s);

			break;

		case TN_DECL_LIST:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_DECLS:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_STEMNT_LIST:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_STEMNT:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_BIT_FIELD:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_PNTR:
			/* pointer - for HW2! */
			if (root->rnode == NULL)
			{
				return 1;
			}
			else
			{
				return 1 + obj_tree(root->rnode);
			}

			break;

		case TN_TYPE_NME:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_INIT_LIST:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_INIT_BLK:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_OBJ_DEF:
			struc = newStruct(((leafnode *)root->lnode)->data.sval->str);
			obj_tree(root->rnode);
			break;

		case TN_OBJ_REF:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_CAST:
			/* Maybe you will use it later */
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			break;

		case TN_JUMP:
			if (root->hdr.tok == RETURN)
			{
				/* return jump - for HW2! */
				obj_tree(root->lnode);
				obj_tree(root->rnode);
			}
			else if (root->hdr.tok == BREAK)
			{
				/* break jump - for HW2! */
				if(printBreak){
					printf("ujp label%d\n", breakLabel);
				}
			}
			else if (root->hdr.tok == GOTO)
			{
				/* GOTO jump - for HW2! */
				obj_tree(root->lnode);
				obj_tree(root->rnode);
			}
			break;

		case TN_SWITCH:
			/* Switch case - for HW2! */
			isConstSwitch = 0;
			if(isConstant(root->lnode)){
				la = printBreak;
				printBreak = 0;
				isConstSwitch = 1;
				switchValue = calculateValue(root->lnode);
				obj_tree(root->rnode);
				printBreak = la;
				break;
			}
			la = LAB++;
			prevBreak = breakLabel;
			breakLabel = la;
			obj_tree(root->lnode);
			obj_tree(root->rnode);
			printf("label%d:\n",la);
			lb = LAB++;
			printf("label%d:\n",lb);
			breakLabel = prevBreak;
			break;

		case TN_INDEX:

			if (root->lnode->hdr.type == TN_DEREF)
			{

				obj_tree(root->lnode);
				obj_tree(root->rnode);
				printf("add\n");
				if (!addAddress)
				{
					printf("ind\n");
				}
				break;
			}

			lb = addAddress;
			if (root->lnode->hdr.type == TN_INDEX)
			{
				la = obj_tree(root->lnode);
			}
			else
			{
				if (root->lnode->hdr.type == TN_SELECT)
				{
					addAddress = 1;
					obj_tree(root->lnode);
					addAddress = lb;
				}
				else
				{
					var = findVar(((leafnode *)root->lnode)->data.sval->str);
					printf("ldc %d\n", var->address);
				}
				sizesOfArr = var->arrSizes;
				la = var->size;
				printf("ldc %d\n", 0);
			}
			la /= sizesOfArr->s;
			addAddress = 0;
			list_ = sizesOfArr;
			obj_tree(root->rnode);
			sizesOfArr = list_;
			addAddress = lb;
			printf("ixa %d\n", la);
			sizesOfArr = sizesOfArr->next;
			if (sizesOfArr == NULL)
			{
				printf("ixa %d\n", sizeofVar(var));
				if (!addAddress)
				{
					printf("ind\n");
				}
			}
			return la;
			break;

		case TN_DEREF:
			/* pointer derefrence - for HW2! */
			lb = addAddress;
			addAddress = 1;
			obj_tree(root->rnode);
			addAddress = lb;
			printf("ind\n");
			break;

		case TN_SELECT:
			/* struct case - for HW2! */
			/* Struct select case "." */
			/* e.g. struct_Variable1.x; */
			lb = addAddress;
			addAddress = 1;
			obj_tree(root->lnode);
			addAddress = lb;

			if (root->hdr.tok == ARROW)
			{
				printf("ind\n");
			}

			struc = findVarStruct(var->type);
			var = findVarInStruct(((leafnode *)root->rnode)->data.sval->str, struc);
			printf("ldc %d\n", var->address);
			printf("add\n");
			if (!addAddress)
			{
				printf("ind\n");
			}
			break;

		case TN_ASSIGN:
			if (root->hdr.tok == EQ)
			{
				lb = addAddress;
				/* Regular assignment "=" */
				/* e.g. x = 5; */
				addAddress = 1;
				obj_tree(root->lnode);
				addAddress = lb;
				obj_tree(root->rnode);
				printf("sto\n");
			}
			else if (root->hdr.tok == PLUS_EQ)
			{
				/* Plus equal assignment "+=" */
				/* e.g. x += 5; */
				if(isConstant(root->rnode) && calculateValue(root->rnode) == 0){
					break;
				}
				lb = addAddress;
				addAddress = 1;
				obj_tree(root->lnode);
				addAddress = lb;
				printf("dpl\n");
				printf("ind\n");
				obj_tree(root->rnode);
				printf("add\n");
				printf("sto\n");
			}
			else if (root->hdr.tok == MINUS_EQ)
			{
				/* Minus equal assignment "-=" */
				/* e.g. x -= 5; */
				if(isConstant(root->rnode) && calculateValue(root->rnode) == 0){
					break;
				}
				lb = addAddress;
				addAddress = 1;
				obj_tree(root->lnode);
				addAddress = lb;
				printf("dpl\n");
				printf("ind\n");
				obj_tree(root->rnode);
				printf("sub\n");
				printf("sto\n");
			}
			else if (root->hdr.tok == STAR_EQ)
			{
				/* Multiply equal assignment "*=" */
				/* e.g. x *= 5; */
				if(isConstant(root->rnode)){
					la = calculateValue(root->rnode);
					if(la == 1){
						break;
					}else if (la == 0){
						lb = addAddress;
						addAddress = 1;
						obj_tree(root->lnode);
						addAddress = lb;
						addStack(0);
						printf("sto\n");
						break;
					}
					
				}
				lb = addAddress;
				addAddress = 1;
				obj_tree(root->lnode);
				addAddress = lb;
				printf("dpl\n");
				printf("ind\n");
				obj_tree(root->rnode);
				printf("mul\n");
				printf("sto\n");
			}
			else if (root->hdr.tok == DIV_EQ)
			{
				/* Divide equal assignment "/=" */
				/* e.g. x /= 5; */
				if(isConstant(root->rnode) && calculateValue(root->rnode) == 1){
					break;
				}
				lb = addAddress;
				addAddress = 1;
				obj_tree(root->lnode);
				addAddress = lb;
				printf("dpl\n");
				printf("ind\n");
				obj_tree(root->rnode);
				printf("div\n");
				printf("sto\n");
			}
			break;

		case TN_EXPR:
			switch (root->hdr.tok)
			{
			case CASE:
				/* you should not get here */

				obj_tree(root->lnode);
				obj_tree(root->rnode);
				break;

			case INCR:
				/* Increment token "++" */

				//++i - after | i++ before
				if (root->lnode == NULL)
				{
					v = findVar(((leafnode *)root->rnode)->data.sval->str);
					left = 0;
				}
				else
				{
					v = findVar(((leafnode *)root->lnode)->data.sval->str);
					left = 1;
				}

				if (left)
				{
					printf("ldc %d\n", v->address);
					printf("ind\n");
				}
				printf("ldc %d\n", v->address);
				printf("ldc %d\n", v->address);
				printf("ind\n");
				printf("inc 1\n");
				printf("sto\n");

				if (!left)
				{
					printf("ldc %d\n", v->address);
					printf("ind\n");
				}
				break;

			case DECR:
				/* Decrement token "--" */
				if (root->lnode == NULL)
				{
					v = findVar(((leafnode *)root->rnode)->data.sval->str);
					left = 0;
				}
				else
				{
					v = findVar(((leafnode *)root->lnode)->data.sval->str);
					left = 1;
				}

				if (left)
				{
					printf("ldc %d\n", v->address);
					printf("ind\n");
				}
				printf("ldc %d\n", v->address);
				printf("ldc %d\n", v->address);
				printf("ind\n");
				printf("dec 1\n");
				printf("sto\n");

				if (!left)
				{
					printf("ldc %d\n", v->address);
					printf("ind\n");
				}
				break;

			case PLUS:
				/* Plus token "+" */

				lb = 1;
				
				if (isConstant(root))
				{
					addStack(calculateValue(root));
					break;
				}
				if (isConstant(root->lnode))
				{
					la = calculateValue(root->lnode);
					if (la != 0)
					{
						addStack(la);
					}
					else
					{
						lb = 0;
					}
				}
				else
				{
					obj_tree(root->lnode);
				}

				if (isConstant(root->rnode))
				{
					la = calculateValue(root->rnode);
					if (la != 0)
					{
						addStack(la);
					}
					else
					{
						lb = 0;
					}
				}
				else
				{
					obj_tree(root->rnode);
				}
				if (lb)
				{
					printf("add\n");
				}
				break;

			case MINUS:
				/* Minus token "-" */

				if (root->lnode == NULL)
				{
					if (isConstant(root))
					{
						addStack(calculateValue(root));
						break;
					}
					obj_tree(root->rnode);
					printf("neg\n");
				}
				else
				{
					/* Plus token "+" */
					if (isConstant(root))
					{
						addStack(calculateValue(root));
						break;
					}

					if (isConstant(root->lnode))
					{
						addStack(calculateValue(root->lnode));
					}
					else
					{
						obj_tree(root->lnode);
					}

					if (isConstant(root->rnode))
					{
						la = calculateValue(root->rnode);
						if (la == 0)
						{
							break;
						}
						addStack(la);
					}
					else
					{
						obj_tree(root->rnode);
					}

					printf("sub\n");
				}

				break;
			case DIV:
				/* Divide token "/" */

				if (isConstant(root))
				{
					addStack(calculateValue(root));
					break;
				}

				if (isConstant(root->lnode))
				{
					la = calculateValue(root->lnode);
					addStack(la);
					if (la == 0)
					{
						break;
					}
				}
				else
				{
					obj_tree(root->lnode);
				}

				if (isConstant(root->rnode))
				{
					la = calculateValue(root->rnode);
					if (la == 1)
					{
						break;
					}
					addStack(la);
				}
				else
				{
					obj_tree(root->rnode);
				}

				printf("div\n");

				break;

			case STAR:
				/* multiply token "*" */

				lb = 1;
				if (isConstant(root))
				{
					addStack(calculateValue(root));
					break;
				}

				if (isConstant(root->lnode))
				{
					la = calculateValue(root->lnode);
					if (la != 1)
					{
						addStack(la);
					}
					else
					{
						lb = 0;
					}
				}
				else
				{
					obj_tree(root->lnode);
				}

				if (isConstant(root->rnode))
				{
					la = calculateValue(root->rnode);
					if (la != 1)
					{
						addStack(la);
					}
					else
					{
						lb = 0;
					}
				}
				else
				{
					obj_tree(root->rnode);
				}
				if (lb)
				{
					printf("mul\n");
				}
				break;

			case AND:
				/* And token "&&" */
				lb = 1;
				if (isConstant(root))
				{
					addStack(calculateValue(root));
					break;
				}

				if (isConstant(root->lnode))
				{
					lb = 0;
				}
				else
				{
					obj_tree(root->lnode);
				}

				if (isConstant(root->rnode))
				{
					lb = 0;
				}
				else
				{
					obj_tree(root->rnode);
				}
				if (lb)
				{
					printf("and\n");
				}
				break;

			case OR:
				/* Or token "||" */
				lb = 1;
				if (isConstant(root))
				{
					addStack(calculateValue(root));
					break;
				}

				if (isConstant(root->lnode))
				{
					lb = 0;
				}
				else
				{
					obj_tree(root->lnode);
				}

				if (isConstant(root->rnode))
				{
					lb = 0;
				}
				else
				{
					obj_tree(root->rnode);
				}
				if (lb)
				{
					printf("or\n");
				}
				break;

			case NOT:
				/* Not token "!" */

				if (isConstant(root))
				{
					addStack(calculateValue(root));
					break;
				}

				obj_tree(root->rnode);
				printf("not\n");
				break;

			case GRTR:
				/* Greater token ">" */
				if (isConstant(root))
				{
					addStack(calculateValue(root));
					break;
				}

				if (isConstant(root->lnode))
				{
					addStack(calculateValue(root->lnode));
				}
				else
				{
					obj_tree(root->lnode);
				}

				if (isConstant(root->rnode))
				{
					addStack(calculateValue(root->rnode));
				}
				else
				{
					obj_tree(root->rnode);
				}

				printf("grt\n");
				break;

			case LESS:
				/* Less token "<" */
				if (isConstant(root))
				{
					addStack(calculateValue(root));
					break;
				}

				if (isConstant(root->lnode))
				{
					addStack(calculateValue(root->lnode));
				}
				else
				{
					obj_tree(root->lnode);
				}

				if (isConstant(root->rnode))
				{
					addStack(calculateValue(root->rnode));
				}
				else
				{
					obj_tree(root->rnode);
				}

				printf("les\n");
				break;

			case EQUAL:
				/* Equal token "==" */
				if (isConstant(root))
				{
					addStack(calculateValue(root));
					break;
				}

				if (isConstant(root->lnode))
				{
					addStack(calculateValue(root->lnode));
				}
				else
				{
					obj_tree(root->lnode);
				}

				if (isConstant(root->rnode))
				{
					addStack(calculateValue(root->rnode));
				}
				else
				{
					obj_tree(root->rnode);
				}

				printf("equ\n");
				break;

			case NOT_EQ:
				/* Not equal token "!=" */
				if (isConstant(root))
				{
					addStack(calculateValue(root));
					break;
				}

				if (isConstant(root->lnode))
				{
					addStack(calculateValue(root->lnode));
				}
				else
				{
					obj_tree(root->lnode);
				}

				if (isConstant(root->rnode))
				{
					addStack(calculateValue(root->rnode));
				}
				else
				{
					obj_tree(root->rnode);
				}

				printf("neq\n");
				break;

			case LESS_EQ:
				/* Less or equal token "<=" */
				if (isConstant(root))
				{
					addStack(calculateValue(root));
					break;
				}

				if (isConstant(root->lnode))
				{
					addStack(calculateValue(root->lnode));
				}
				else
				{
					obj_tree(root->lnode);
				}

				if (isConstant(root->rnode))
				{
					addStack(calculateValue(root->rnode));
				}
				else
				{
					obj_tree(root->rnode);
				}

				printf("leq\n");
				break;

			case GRTR_EQ:
				/* Greater or equal token ">=" */
				if (isConstant(root))
				{
					addStack(calculateValue(root));
					break;
				}

				if (isConstant(root->lnode))
				{
					addStack(calculateValue(root->lnode));
				}
				else
				{
					obj_tree(root->lnode);
				}

				if (isConstant(root->rnode))
				{
					addStack(calculateValue(root->rnode));
				}
				else
				{
					obj_tree(root->rnode);
				}

				printf("geq\n");
				break;

			default:
				obj_tree(root->lnode);
				obj_tree(root->rnode);
				break;
			}
			break;

		case TN_WHILE:
			/* While case */
			la = LAB++;
			lb = LAB++;
			prevBreak = breakLabel;
			breakLabel = lb;
			printf("label%d:\n", la);
			obj_tree(root->lnode);
			printf("fjp label%d\n", lb);
			obj_tree(root->rnode);
			printf("ujp label%d\n", la);
			printf("label%d:\n", lb);
			breakLabel = prevBreak;
			break;

		case TN_DOWHILE:
			la = LAB++;
			lb = LAB++;
			prevBreak = breakLabel;
			breakLabel = lb;
			printf("label%d:\n", la);
			obj_tree(root->rnode);
			obj_tree(root->lnode);
			printf("not\n");
			printf("fjp label%d\n", la);
			printf("label%d:\n", lb);
			breakLabel = prevBreak;
			break;

		case TN_LABEL:
			if(isConstSwitch){
				if(calculateValue(root->lnode->rnode) == switchValue){
					obj_tree(root->rnode);
				}
				break;
			}
			la = LAB++;
			//get num_i
			printf("label%d:\n", la);
			printf("dpl\n");
			obj_tree(root->lnode);
			printf("equ\n");
			printf("fjp label%d\n", la + 1);

			obj_tree(root->rnode);

			break;

		default:
			obj_tree(root->lnode);
			obj_tree(root->rnode);
		}
		break;

	case NONE_T:
		printf("Error: Unknown node type!\n");
		exit(FAILURE);
	}

	return SUCCESS;
}


/*
*	This function prints all the Variable1s on your symbol table with their data
*	Input: treenode (AST)
*	Output: prints the Sumbol Table on the console
*/
void print_symbol_table(treenode* root) {}