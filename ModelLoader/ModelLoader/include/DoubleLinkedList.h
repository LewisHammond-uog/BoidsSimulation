#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

/// <summary>
/// Template class for a doubl-y linked list
/// </summary>
template <class T>
class DoubleLinkedList
{
public:
	
	/// <summary>
	/// Node for a double linked list
	/// With a prev and next
	/// </summary>
	struct Node
	{
		T* m_pData;
		Node* m_pPrev; //Previous node in the list
		Node* m_pNext; //Next node in the list
	};

	//Constructor and destructor
	DoubleLinkedList();
	~DoubleLinkedList();

	void Push(T* a_pData);
	void Pop(T* a_pData);
	T* Search(unsigned int a_iPos);
	void Clear();
	
private:
	Node* m_pHead; //Start of linked list

};

//Constructor
template <class T>
DoubleLinkedList<T>::DoubleLinkedList() :
	m_pHead(nullptr)
{
}

//Destructor
template <class T>
DoubleLinkedList<T>::~DoubleLinkedList()
{
	//On Destroy remove all nodes from the list
	Clear();
}

/// <summary>
/// Pushes a node to the front
/// of the linked list
/// </summary>
template <class T>
void DoubleLinkedList<T>::Push(T* a_pData)
{
	//Fill with data
	Node* pNewNode = new Node();
	pNewNode->m_pData = a_pData;
	
	//Make the next of the new node point to the current head
	pNewNode->m_pNext = m_pHead;
	pNewNode->m_pPrev = nullptr;

	//Set the head's prev to be this
	if(m_pHead != nullptr)
	{
		m_pHead->m_pPrev = pNewNode;
	}

	//Set this as the new head
	m_pHead = pNewNode;
}

/// <summary>
/// Remove a node with given data from the list
/// </summary>
template <class T>
void DoubleLinkedList<T>::Pop(T* a_pData)
{
	//Store the current search node
	Node* currentNode = m_pHead;

	//Check if it is the head we are deleting
	if(currentNode != nullptr && currentNode->m_pData == a_pData)
	{
		//Set the new head of the list
		m_pHead = currentNode->m_pNext;

		//Set the heads prev to be null
		if(m_pHead)
		{
			m_pHead->m_pPrev = nullptr;
		}

		delete currentNode;
		return;
	}

	//Search through our list for the data to be deleted
	while(currentNode != nullptr && currentNode->m_pData != a_pData)
	{
		currentNode = currentNode->m_pNext;
	}

	//If we are reached the end of the list then the data is not in the list
	//early out
	if(currentNode == nullptr)
	{
		return;
	}

	//Key was present in the list and it is currentNode
	//Unlink from the list so our structure looks like
	//[currentNode.prev] <-> [currentNode] <-> [currentNode.next]
	//[currentNode.prev] <-> [currentNode.next]
	currentNode->m_pPrev->m_pNext = currentNode->m_pNext; //It's previous next should now be current nodes next
	currentNode->m_pNext->m_pPrev = currentNode->m_pPrev; //It's next's prevouus should now be our current nodes previous

	//Delete our current node
	delete currentNode;
}

/// <summary>
/// Searches for a boid at a given position in the list
/// </summary>
template <class T>
T* DoubleLinkedList<T>::Search(const unsigned int a_iPos)
{
	Node* pCurrentNode = m_pHead;

	//Loop through the list to find node at index,
	//while we still have nodes in the list
	unsigned int iCurrentIndex = 0;
	while (pCurrentNode != nullptr)
	{
		//Found
		if(iCurrentIndex == a_iPos)
		{
			return pCurrentNode->m_pData;
		}

		//Not found increment and contiue looping
		iCurrentIndex++;
		pCurrentNode = pCurrentNode->m_pNext;
	}

	//No node at the given position int the list (i.e over list size)
	return nullptr;
}

/// <summary>
/// Deletes all of the nodes in the list
/// </summary>
template <class T>
void DoubleLinkedList<T>::Clear()
{
	//Starting at the head loop and delete nodes until we reach the end node
	Node* currentNode = m_pHead;
	while(currentNode != nullptr)
	{
		Node* nextNode = currentNode->m_pNext;
		delete currentNode;
		currentNode = nextNode;	
	}
	
	//Clear head
	m_pHead = nullptr;
}


#endif //!__LINKED_LIST_H__

