#include <iostream>
#include <string>
#include <algorithm>

void clearExtra()
{
  // ignores all of the characters up until newline
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

class Item
{
  private:
    int m_id;
    std::string m_name;
    int m_quantity;
    double m_price;

  public:
    Item() = default;

    void create_item()
    {
      bool _isNumeric {false};

      std::cout << "Enter name: ";
      std::getline(std::cin, m_name);
      if(std::cin.fail())
      {
        std::cin.clear();
        clearExtra();
      }
      std::cout << "Enter ID: ";
      if(std::cin >> m_id)
      {
        _isNumeric = true;
      } else {
        _isNumeric = false;
        std::cin.clear();
        clearExtra();
      }

      std::cout << "Enter price per item ($): ";
      std::cin >> m_price;
      std::cout << "Enter quantity currently in stock: ";
      std::cin >> m_quantity;
    }

    void print_details()
    {
      std::cout << "================== \n";
      std::cout << "Name: " << m_name <<
      "\n ID: " << m_id << "\n Stock: " << m_quantity
      << "\n Price: $" << m_price << '\n';
      std::cout << "================== \n";
    }
    
    void change_name()
    {
      std::cout << "Current name: " << m_name
      << "\n Change name to: ";
      std::cin >> m_name;
    }

    int get_id() {return m_id;}
    std::string get_name() {return m_name;}
    int get_qty() {return m_quantity;}
    double get_price() {return m_price;}
};




int main()
{
  Item it1{};
  it1.create_item();
  it1.print_details();
  return 0;
}
