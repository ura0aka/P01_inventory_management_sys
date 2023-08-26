#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <stack>
#include <sstream>

class Item;
class Container;


void clear_extra()
{
  // ignores all of the characters up until newline
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

template <typename T>
T prompt_for_numeric(std::string message)
{
  while(1)
  {
    T tEntry;
    bool bIsNumeric {false};
    std::cout << message;
    if(std::cin >> tEntry)
    {
      bIsNumeric = true;
      if(tEntry < 0)
      {
        std::cerr << "Cannot be (less than) < 0.\n";
        continue;
      }
      clear_extra();
      return tEntry;
    } else {
      bIsNumeric = false;
      std::cerr << "ERROR: Invalid entry.\n";
      std::cin.clear();
      clear_extra();
      continue;
    }
  }
}


class Datafile
{
  public:
    inline void set_string(const std::string& sString, std::size_t nItem = 0)
    {
      // sets string value of property (for given index, zero by default)
      if(nItem >= m_vContent.size())
        m_vContent.resize(nItem + 1);
      m_vContent[nItem] = sString;
    }

    inline const std::string get_string(std::size_t nItem = 0) const
    {
      // returns string at given index or "" if given index is >= vec size
      if(nItem >= m_vContent.size())
        return "";
      else
      return m_vContent[nItem];
    }
    
    // == real numbers ==
    inline void set_real(const float f, const std::size_t nItem = 0)
    {
      set_string(std::to_string(f),nItem);
    }

    inline const float get_real(std::size_t nItem = 0) const
    {
      return std::atof(get_string(nItem).c_str());
    }

    // == integers ==
    inline void set_int(const int n, const std::size_t nItem = 0)
    {
      set_string(std::to_string(n),nItem);
    }

    inline const int get_int(std::size_t nItem = 0) const
    {
      return std::atoi(get_string(nItem).c_str());
    }

    inline std::size_t get_size() const {return m_vContent.size();}

    inline Datafile& operator[] (const std::string& name)
    {
      // check if node's map already contains the object with the given name
      if(m_mapObjects.count(name) == 0)
      {
        // if it does not exist: create object in map, then link the vector index with object name
        m_mapObjects[name] = m_vecObjects.size();
        // then create empty object in the vector of objects
        m_vecObjects.push_back({name, Datafile()});
      }
      // if exists: return the object by getting its index from the map (m_mapObjects)
      // we use the index to look up the element in the vector of objets (m_vecObjects)
      // essentially what we return is: std::vector<std::pair<std::string, *DATAFILE*>>
      return m_vecObjects[m_mapObjects[name]].second;
    }

    // our datafile format for this project:
// "n_sItemName"
//  {
//    m_nId = 5
//    m_nQuantity = 489
//    m_fPrice = 4.90
//  }

    // == Writing to file ==
    inline static bool write_to_file(const Datafile& n, const std::string& sFileName,
        const std::string& sIndent = "\t")
    {
      // lambda function to let datafile write itself recursively
      std::function<void(const Datafile&, std::ofstream&)> write =
      [&](const Datafile& n, std::ofstream& file)
      {
        for(auto const& property : n.m_vecObjects)
        {
          if(property.second.m_vecObjects.empty())
          {
            file << property.first << " : ";
            std::size_t nItems = property.second.get_size();
            for(std::size_t i{0}; i < nItems; i++)
            {
              file << property.second.get_string(i);
            }
            file << '\n';
          }
          else
          {
            file << sIndent << property.first << '\n';
            // open braces to accomodate children and indent (for formatting)
            file << sIndent << "{\n";
            write(property.second, file);
            file << sIndent << "}\n\n";
          }
        }
      };

      std::ofstream file(sFileName);
      if(file.is_open())
      {
        write(n,file);
        return true;
      }
      return false; // there was an error opening or writing to the file
    }

  private:
    std::vector<std::string> m_vContent {};
    std::vector<std::pair<std::string, Datafile>> m_vecObjects {};
    std::unordered_map<std::string, std::size_t> m_mapObjects {};
};



class Item
{
  public:
    Item() : m_nId{m_nIdGenerator ++}
    {}

    friend std::ostream& operator<< (std::ostream& out, const Item& item)
    {
      out << item.m_sItemName;
      return out;
    }
    
    void create_item()
    {
      bool bIsNumeric {false};

      std::cout << ">> Enter name: ";
      std::getline(std::cin, m_sItemName);
      if(std::cin.fail())
      {
        std::cin.clear();
        clear_extra();
      }
      m_fPrice = prompt_for_numeric<float>(">> Enter price ($): ");
      m_nQuantity = prompt_for_numeric<int>(">> Enter stock: ");
    }
    
    void print_details()
    {
      std::cout << "Name: " << m_sItemName << "\n Id: " << m_nId <<
        "\n Quantity: " << m_nQuantity << "\n Price: " << m_fPrice << '\n';
    }

    void set_id(int id) {m_nId = id;}
    void set_name(std::string str) {m_sItemName = str;}
    void set_price(int price) {m_fPrice = price;}
    void set_qty(int qty) {m_nQuantity = qty;}

    int get_id() {return m_nId;}
    std::string get_name() {return m_sItemName;}
    float get_price() {return m_fPrice;}
    int get_qty() {return m_nQuantity;}

    void update_quantity_add(int qty) {m_nQuantity += qty;}
    void update_quantity_subtract(int qty)
    {
      qty <= m_nQuantity ? (m_nQuantity -= qty) : (m_nQuantity = 0);
    }

    void update_name()
    {
      clear_extra();
      std::cout << "Current name: " << m_sItemName
        << "\n>> Change name to: ";
      std::getline(std::cin, m_sItemName);
      if(std::cin.fail())
      {
        std::cin.clear();
        clear_extra();
      }
    }

  private:
    static inline int m_nIdGenerator {1};
    int m_nId;
    std::string m_sItemName;
    int m_nQuantity;
    float m_fPrice;
};


class Container
{
  std::vector<Item> m_vItemVec{};

  public:
    Container() = default;

    void add_item(Item& item)
    {
      m_vItemVec.emplace_back(item);
    }

    void display_items()
    {
      for(auto& itm : m_vItemVec)
        itm.print_details();
    }

    std::vector<Item>& get_list() {return m_vItemVec;}
};


int main()
{
  Item i1;
  Container c1;
  Datafile df;

  i1.create_item();
  i1.print_details();

  Item i2;
  i2.create_item();

  c1.add_item(i1);
  c1.add_item(i2);
  c1.display_items();
  
  df[i1.get_name()]["ID"].set_int(i1.get_id()); 
  df[i1.get_name()]["Quantity"].set_int(i1.get_qty()); 
  df[i1.get_name()]["Price"].set_int(i1.get_price()); 

  df[i2.get_name()]["ID"].set_int(i2.get_id()); 
  df[i2.get_name()]["Quantity"].set_int(i2.get_qty()); 
  df[i2.get_name()]["Price"].set_int(i2.get_price());

  Datafile::write_to_file(df,"test_output1.txt");
  return 0;
}
