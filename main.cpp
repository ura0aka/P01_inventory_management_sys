#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <map>
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

int count_lines_in_file(const std::string& sFileName)
{    
  std::ifstream file;
  const int item_offset{7};
  int count_line = 0, item_count = 0;
  std::string line;
  
  file.open(sFileName);
  if(file.is_open())
  {
    while(std::getline(file,line))
    {
      ++count_line;
    }
    item_count = count_line/item_offset;
  }
  return item_count;
}


class Item
{
  public:
    Item() : m_nId{m_nIdGenerator += 1}
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
    static inline int m_nIdGenerator {0};
    int m_nId;
    std::string m_sItemName;
    int m_nQuantity;
    float m_fPrice;
};


class Container
{
  std::map<int,Item> m_mapItems{}; // item ID points to the item object in map

  public:
    Container() = default;

    void add_item(Item& item)
    {
      m_mapItems[item.get_id()] = item; 
    }

    void display_items()
    {
      for(std::size_t i{1}; i != m_mapItems.size()+1; ++i)
      {
        m_mapItems[i].print_details();
      }
    }

    std::map<int,Item> get_map() {return m_mapItems;}
};


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
      std::size_t nIndentCount = 0; // tracks the level of indentation in out output

      // lambda function to let datafile write itself recursively
      std::function<void(const Datafile&, std::ofstream&)> write =
      [&](const Datafile& n, std::ofstream& file)
      {
        auto indent = [&](const std::string& sString, const std::size_t nCount)
        {
          std::string sOut;
          for(std::size_t n{0}; n < nCount; n++)
            sOut += sString;
          return sOut;
        };

        // iterate through each property of the node
        for(auto const& property : n.m_vecObjects)
        {
          if(property.second.m_vecObjects.empty())
          {
            file << indent(sIndent, nIndentCount) << property.first << " : ";
            std::size_t nItems = property.second.get_size();
            for(std::size_t i{0}; i < nItems; i++)
            {
              file << property.second.get_string(i);
            }
            file << '\n';
          }
          else
          {
            file << indent(sIndent, nIndentCount) << property.first << '\n';
            // open braces to accomodate children and indent (for formatting)
            file << indent(sIndent, nIndentCount) << "{\n";
            nIndentCount ++;
            // write out node recursively
            write(property.second, file);
            file << indent(sIndent, nIndentCount) << "}\n\n";
          }
        }
        if(nIndentCount > 0)
          nIndentCount --;
      };

      std::ofstream file(sFileName);
      if(file.is_open())
      {
        write(n,file);
        return true;
      }
      return false; // there was an error opening or writing to the file
    }


  // reading from file into the program
  inline static bool read_from_file(const std::string& sFileName, Container& cont)
  {
    std::ifstream file(sFileName);
    std::string sPropertyValue = "";
    const int line_offset{7};
    int item_count = count_lines_in_file(sFileName);
    int count = 0, line_count = 0;     
    bool bItemsCreated {false};

    if(file.is_open())
    {   
      // lambda to trim line
      auto trim = [](std::string& s)
      {
        s.erase(0,s.find_first_not_of(" \t\n\r\f\v")); // erase portion of string from index zero to the specified 
        s.erase(s.find_last_not_of(" \t\n\r\f\v")+1);        
      };

      // lambda to go to specified line & return the line's property value
      auto go_to_line = [&](std::ifstream& f, unsigned int num, bool bTrimFlag)
      {
        std::string line;
        f.seekg(std::ios::beg);
        for(int i{0}; i < num; ++i)
        {
          f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::getline(f,line);
          
        if(bTrimFlag == true) // ignore how horrible this is... please
        {
          if(!line.empty())
          {
            std::size_t x = line.find_first_of(":");
            if(x != std::string::npos)
            {
              line = line.substr(x+1, line.size());
              trim(line);
            }
          }
        }
        return line;
      };
        
      while(!bItemsCreated)
      {
        // all items are written in file in a specific order/format (each line is reserved for a specific item property)
        Item tmp_item{};
        tmp_item.set_name(go_to_line(file,0+line_count,0));
        tmp_item.set_id(std::stoi(go_to_line(file,2+line_count,1)));
        tmp_item.set_qty(std::stoi(go_to_line(file,3+line_count,1)));
        tmp_item.set_price(std::stof(go_to_line(file,4+line_count,1)));
        cont.add_item(tmp_item);
        
        ++count;
        line_count += line_offset;
        if(item_count == count)
          bItemsCreated = true;
      }
    }
    else
    {
      return false; // error, could not read from specified file
    }
    file.close();
    return true;
  }


  private:
    std::vector<std::string> m_vContent {};
    std::vector<std::pair<std::string, Datafile>> m_vecObjects {};
    std::unordered_map<std::string, std::size_t> m_mapObjects {};
};

void add_sales(Container& cont)
{
  std::map<int,Item> mapItemList = cont.get_map();
  float fTotalPrice;
  cont.display_items();

  char cQuery{'y'};
  while(cQuery == 'y' | cQuery == 'Y')
  {
    int nId = prompt_for_numeric<int>("Enter item ID: ");
    bool bValid{false};
    if(nId == mapItemList[nId].get_id())
    {      
      while(!bValid)
      {
        int nQty = prompt_for_numeric<int>("Enter quantity: ");
        if(nQty > mapItemList[nId].get_qty()) 
        {
          std::cerr << "ERROR: Not enough in stock \n";
          continue;
        }
        else if(nQty <= mapItemList[nId].get_qty())
        {
          fTotalPrice += (nQty * mapItemList[nId].get_price());
          mapItemList[nId].update_quantity_subtract(nQty);
          bValid = true;
        }
      }
    }
    else
    {
      std::cerr << "ERROR: Provided ID does not exist \n";
    }
  
    if(bValid == true)
    {
    std::cout << "Add more items to order? (y/n): ";
      std::cin >> cQuery;
    }
    std::cout << "Total bill: $" << fTotalPrice << '\n';
  }
}

int main()
{
  /*    
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
  df[i1.get_name()]["Price"].set_real(i1.get_price()); 

  df[i2.get_name()]["ID"].set_int(i2.get_id()); 
  df[i2.get_name()]["Quantity"].set_int(i2.get_qty()); 
  df[i2.get_name()]["Price"].set_real(i2.get_price());
  */

  Datafile df;
  Container c1;

  //Datafile::write_to_file(df,"test_output1.txt");

  Datafile::read_from_file("test_output1.txt", c1);
  c1.display_items();

  return 0;
}
