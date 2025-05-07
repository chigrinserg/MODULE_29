#include <iostream>
#include <fstream>
#include <mutex>
#include <string>

struct Node
{
	std::string value; //хранимое значение
	Node* next; //указатель на следующий узел
	Node(std::string _val) :value(_val), next(nullptr) {} //конструктор
	std::mutex* node_mutex = new std::mutex;

};

class FineGrainedQueue
{
public:
	FineGrainedQueue(){};
	~FineGrainedQueue(){};
	bool is_empty();
	void push_back(std::string value);
	void print();
	void insertIntoMiddle(std::string value, int pos);
private:
	Node* first;
	Node* last;
	std::mutex* queue_mutex = new std::mutex;
};

bool FineGrainedQueue::is_empty() //ПРОВЕРКА НАЛИЧИЯ УЗЛОВ В СПИСКЕ
{
	return first == nullptr;
}

void FineGrainedQueue::print() //ФУНКЦИЯ ПЕЧАТИ СПИСКА
{
	if (is_empty()) return;
	Node* p = first; //создаем указатель на первый узел 
	while (p) // пока p != nullptr
	{
		std::cout << p->value << "  "; //выводим значение узла, пока p не пустой
		p = p->next; //при каждой итерации обновляем значение p на следующий узел.
	}
}

void FineGrainedQueue::push_back(std::string _val) //ДОБАВЛЕНИЕ ЭЛЕМЕНТА В КОНЕЦ СПИСКА
{
	Node* p = new Node(_val); //создаем узел со значением, которое мы передали в функцию
	if (is_empty())
	{
		first = p; //присваиваем указателю на первый и последний узлы указатель на новый узел
		last = p;
		return;
	}
	last->next = p; //указываем, что новый узел стоит после последнего узла.
	last = p; //Затем мы меняем значения указателя last.
}

void FineGrainedQueue::insertIntoMiddle( std::string value, int pos) //ФУНКЦИЯ ВСТАВКИ НОВОГО УЗЛА
{
	Node* newNode = new Node(value); // создаем новый узел 

	std::unique_lock<std::mutex> lock(FineGrainedQueue* queue_mutex);

	if (first == nullptr)
	{
		first = newNode; // если список пуст, новый узел и будет началом списка
		return;
	}
	else if (pos == 0)
	{
		newNode->next = first; // крайний случай - вставка  в начало списка
		first = newNode;
		return;
	}
	std::unique_lock<std::mutex> unlock(FineGrainedQueue* queue_mutex);

	int currPos = 0;
	
	Node* current = first;
	std::lock_guard<std::mutex> lock(Node * node_mutex);
	// в цикле идем по списку, пока список не кончится, или пока не дойдем до позиции
	while (currPos < pos - 1 && current->next != nullptr)
	{
		current = current->next;
		currPos++;
	}
	
	Node* next = current->next; // меняем указатель на следующий узел на указатель на новый узел
	//next->node_mutex->lock();
	current->next = newNode;
	newNode->next = next; /*Связываем список обратно, меняем указатель на узел, следующий после нового узла,
						  на указатель на узел, следующий за current*/
	//next->node_mutex->unlock();
}

int countNumber() //Вспомогателльная функция-счетчик числа запусков программы
{
	int count{};
	std::ifstream in("Numbers.txt"); // окрываем файл для чтения
	if (in.is_open())
	{
		in >> count;
	}
	in.close();	 // закрываем файл
	++count;
	std::ofstream out;		  // поток для записи
	out.open("Numbers.txt");	  // открываем файл для записи
	if (out.is_open())
	{
		out << count << std::endl;
	}
	out.close();
	return count;
}


int main()
{
	setlocale(LC_ALL, "Rus");
	std::cout <<"Запуск N "<<countNumber() << std::endl;
	FineGrainedQueue f;
	f.push_back("Николай");
	f.push_back("Василий");
	f.push_back("Петр");
	f.push_back("Ольга");
	f.push_back("Галина");
	f.print();
	std::cout << '\n';
	std::thread t1(&FineGrainedQueue::insertIntoMiddle, &f, "Константин", 0);
	std::thread t2(&FineGrainedQueue::insertIntoMiddle, &f, "Игорь", 2);
	std::thread t3(&FineGrainedQueue::insertIntoMiddle, &f, "Георгий", 12);
	t1.join();
	t2.join();
	t3.join();
	f.print();
	std::cout << '\n';
	return 0;
}

