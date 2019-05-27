//这是基于孩子兄弟表示法的树类Tree的定义与实现(教材P.164~P.171)
#ifndef TREE_H
#define TREE_H
#include "stdlib.h"
//#include "iostream.h"

template<class T> class Tree;      //前视声明,TreeNode类要申明它为友元类

template<class T> 
class TreeNode           //树结点类(用孩子兄弟表示的树)
{
  friend class Tree<T>;
  private:
	  T data;                  //数据域(原教材放在public段)
	  TreeNode<T> *firstChild;   //指向第一个孩子
	  TreeNode<T> *nextSibling;   //指向下一个兄弟
  public:
	  //构造数据为value第一个孩子fc、下一个兄弟为ns的树结点
	  TreeNode(T value,TreeNode<T> *fc=NULL, TreeNode<T> *ns=NULL)
	  { data=value; firstChild=fc; nextSibling=ns;  }
	  TreeNode<T>* &FirstChild(void)   //返回指向第一个孩子的指针引用
	  { return firstChild; }
	  TreeNode<T>* &NextSibling(void)//返回指向下一个兄弟的指针引用
	  { return nextSibling; }
	  T Getdata(void) const          //获取结点的数据(原教材没有此成员)
	  { return data; }
	  void Setdata(T val) 
	  { return data = val; }
};

template<class T>
class Tree                  //树类Tree的定义
{
private:
	TreeNode<T> *root;     //指向根结点
	TreeNode<T> *current;  //指向当前结点

	void PreOrderTree(TreeNode<T>* &t, TreeNode<T>* &parent); //先根遍历以t为根的树结点
	void PostOrderTree(TreeNode<T>* &t, TreeNode<T>* &parent);   //后根遍历以t为根的树结点
	bool Current(TreeNode<T>* &t)     
	//使t所指结点变为当前结点：成功，返回true;否则返回false.
	{ 
		if(t==NULL) return false;
		current=t;
		return true;
	}
	//在树root中查找结点s的双亲结点
	TreeNode<T> *SearchParent(TreeNode<T> * &root, TreeNode<T>* &s);
public:
	Tree()
	{  root=current=NULL;  }

	~Tree()
	{
		DeleteSubTree(root);
	}

	T GetData()     //取当前结点数据
	{   return current->data;	}

	TreeNode<T>* Root()        
	//使根结点为当前结点:成功，返回true,否则返回false
	{
		current=root;
		return current;
	}

	TreeNode<T>* Current()
	{
		return current;
	}

	bool Parent();               //使当前结点的双亲为当前结点
	bool FirstChild();       //使当前结点的第一个孩子为当前结点
	bool NextSibling();      //使当前结点的下一个兄弟为当前结点

	void InsertChild(T value, bool bUpdateCurrent = true); //把value插入到当前结点最后一个孩子之后
	void DeleteSubTree(TreeNode<T>* &t);//删除以t为根的子树
	bool DeleteChild(int i); //删除当前结点的第i个孩子

    bool Find(TreeNode<T> *root,T item);//在root树中查找其数据为item的结点
	void DisplayTree(int i);//遍历树：i=1为先根遍历；i=2为后根遍历

	void PreOrderTreeFromCurrentNode();
	int  GetChildNoFromCurrent(TreeNode<T>* t);		// 决定 t 是当前节点的第几个孩子
	
	virtual void TraverseOutput(TreeNode<T>* &t, TreeNode<T>* &parent){};
};

template<class T>
void Tree<T>::DeleteSubTree(TreeNode<T>* &t)
{//删除以t为根的子树:实际上是用后根序删除
	if(t==NULL)
	{
		cout<<"The tree is empty!"<<endl;
		return;
	}
	TreeNode<T> *q=t->firstChild, *p;
	while(q!=NULL)   //递归调用本函数逐个删除"第一个孩子"和"下一个兄弟"为根的子树
	{
		p=q->nextSibling;
		DeleteSubTree(q);  //递归调用本函数:删除q所指的子树
		q=p; 
	}
    //cout<<t->data<<"  ";         //教材原来没有此句
	delete t;  //最后删除根结点
	t=NULL;
}

template<class T>
void Tree<T>::PreOrderTree(TreeNode<T>* &t, TreeNode<T>* &parent)
{//先根遍历以t为根的树结点
	if(t==NULL) return;
	
	//cout<<t->data<<" ";    //先输出根t的数据
	TraverseOutput(t, parent);

    //再"先根遍历"以第一个孩子结点为根的子树
	 TreeNode<T> *p = t->firstChild;
	while(p!=NULL)
	{
		 PreOrderTree(p, t);
		 p = p->nextSibling;
	}
}

template<class T>
TreeNode<T>* Tree<T>::SearchParent(TreeNode<T> * &root, TreeNode<T>* &s)
{	
	/*先根后孩子法遍历*/

	//在树 root 中查找结点 s 的双亲结点
	if((root==NULL)||(root==s)) return NULL;
    TreeNode<T> *p=root->FirstChild();    //p指向第一个孩子
     //如果s就是第一个孩子或第一个孩子的兄弟，则根root合所求
	while(p!=NULL&&p!=s) p=p->NextSibling();//在此改正了原教材的错误
	if(p==s) return root;
	
	//在以第一个孩子为根的子树中查找s的双新结点
	TreeNode<T> * pChild = root->FirstChild();
	while(pChild!=NULL)
	{
		p=SearchParent(pChild,s);
		if(p) 
			return p;
		else
			pChild = pChild->NextSibling();
	}
	return NULL;   //查找失败

}

template<class T>
bool Tree<T>::Parent()  
//使当前结点的双亲为当前结点: 成功返回true,否则返回fase
{
 	if(current==NULL) return false;
	
	TreeNode<T> *p=SearchParent(root,current);   //在root树中查找current的双亲
	if(p==NULL) return false;      //查找失败
	else return Current(p);   //查找成功则把它变为当前结点
}

template<class T>
bool Tree<T>::FirstChild()//使当前结点的第一个孩子为当前结点
{
	if(current!=NULL && current->firstChild!=NULL) 
		return Current(current->firstChild);
	else return false;
}

template<class T>
bool Tree<T>::NextSibling()
{//使当前结点的下一个兄弟为当前结点
    if(current!=NULL && current->nextSibling!=NULL) 
		return Current(current->nextSibling);
	else return false;
}

template<class T>
void Tree<T>::InsertChild(T value, bool bUpdateCurrent)
{//把value插入到当前结点最后一个孩子之后
	TreeNode<T> *newNode=new TreeNode<T>(value);    //创建一个新结点

	if(root==NULL)      //如果原树为空，则新结点成为只有根树，亲完成插入操作
	{  root=current=newNode;  return;  }

	if(current->firstChild==NULL) current->firstChild=newNode;//当没有第一个孩子时
	else  //当有第一个孩子时
	{
		TreeNode<T> *p=current->firstChild;   //p指向第一个孩子

		while(p->nextSibling!=NULL) p=p->nextSibling;  //寻找最后一个兄弟
		p->nextSibling=newNode;    //新结点插入最后一个兄弟之后
	}

	if(bUpdateCurrent)
		Current(newNode);     //把新结点作为当前结点
	return;
}

template<class T>
bool Tree<T>::DeleteChild(int i)   //删除当前结点的第i个孩子结点
{
	TreeNode<T> *r=NULL;       //r将用来指向要删除的孩子

	if(i==1)   //如果是删除第一个孩子
	{
		r=current->firstChild;   //指向第一个孩子
		if(r==NULL) return false;  //当没有第一个孩子时删除失败
		current->firstChild=r->nextSibling;//越过r链接
	}
	else    //删除其它孩子
	{
		int k=1;
		TreeNode<T> *p=current->firstChild;
		while(p!=NULL && k<i-1)  //寻找第i个孩子:在此纠正了原教材的错误
		{	p=p->nextSibling;  k++;   }

		if(p!=NULL)
		{
			r=p->nextSibling;
			if(r!=NULL) 
			{
				p->nextSibling=r->nextSibling;  //越过r链接
                r->nextSibling=NULL;
			}
			else return false;   //没有第i个孩子，删除失败
		}
		else return false;  //没有其它孩子，删除失败
	}

		DeleteSubTree(r);    //删除r所指的子树
		cout<<endl;
		return true;     //删除成功
}

template<class T>
bool Tree<T>::Find(TreeNode<T> *root,T item) 
//查找其数据域为item结点，成功则返回true,且当前指针指向该结点，否则返回false
{
	if(root==NULL) return false;
	if(root->data==item)
	{
		current=root;   //当前指针指向找到的结点
		return true;
	}
    bool found=false;
	TreeNode<T> *p=root->FirstChild();
    if(p==NULL) return false;	
    found=Find(p,item);    //到第一个孩子所指的子树中去找
	if(found) return true;   //如果找到了,以成功返回
   //到下一个兄弟所指的子树中去找
	do{
       p=p->NextSibling();
       found=Find(p,item);
	   if(found) break;
	}while(p!=NULL);
	return found;
}

template<class T>
void Tree<T>::DisplayTree(int i)
{ //遍历树：i=1为先根遍历；i=2为后根遍历
	TreeNode<T>* parent = NULL;
	if(i==1) PreOrderTree(root, parent);//调用私有成员函数PreOrderTree
	else PostOrderTree(root, parent);  //调用私有成员函数PostOrderTree
	cout<<endl;
}

template<class T>
void Tree<T>::PreOrderTreeFromCurrentNode()
{
	TreeNode<T>* parent = NULL;
	PreOrderTree(current, parent);//调用私有成员函数PreOrderTree
}

template<class T>
void Tree<T>::PostOrderTree(TreeNode<T>* &t, TreeNode<T>* &parent)
{//后根遍历以t为根的树结点
	if(t==NULL) return;
	//先"后根遍历"以第一个孩子结点为根的树结点
	if(t->firstChild!=NULL) PostOrderTree(t->firstChild, t);
    //显示输出根的数据
	//cout<<t->data<<"  ";
	TraverseOutput(t, parent);
    //最后"后根遍历"以下一个兄弟结点为根的树结点
	if(t->nextSibling!=NULL) PostOrderTree(t->nextSibling, t);
}

template<class T>
int Tree<T>::GetChildNoFromCurrent(TreeNode<T>* t)
{
	int k=1;
	TreeNode<T> *p=current->firstChild;
	while(p!=NULL)  //寻找第i个孩子:在此纠正了原教材的错误
	{
		if(p==t)
			break;
		p=p->nextSibling;  k++;
	}
	
	if(p==NULL)
		return 0;
	else
		return k;
}

#endif //TREE_H

	

