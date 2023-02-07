// Disclaimer: this code 1) is supposed to be spread across
// several .hpp and .cpp files and 2) is not yet polished
// the way I'd want it to. However, I decided giving in
// * something * is more important than reaching perfection...
// We do that in gamedev, am I right?
// I apologize for any possible eye- or headache of anyone reading this code.
// For any questions please contact @Fourth_Rome

#include <cstdio>
#include <iostream>
#include <limits.h>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>


namespace task_1 {
const size_t BitsInInt = sizeof(int) * 8;  // We assume 8-bit bytes.

// First attempt, quickest & shortest.
std::string ToBinary(int n) {
    unsigned n_raw = n;
    std:: string result(BitsInInt, '0');
    int idx = result.size() - 1;
    while (n_raw > 0) {
        if (n_raw % 2 == 1) {
            result[idx] = '1';
        }
        n_raw >>= 1;
        idx -= 1;
    }
    return result;
}

// Second attempt, if we actually want no trailing zeroes.
std::string ToBinaryNoTrailing(int n) {
    unsigned n_raw = n;
    size_t highest_power = BitsInInt - 1;
    unsigned mask = 0x1 << highest_power;
    while ((n_raw & mask) == 0 && (highest_power > 0)) {
        highest_power -= 1;
        mask >>= 1;
    }
    std::string result(highest_power + 1, '0');
    int idx = result.size() - 1;
    while (n_raw > 0) {
        if (n_raw % 2 == 1) {
            result[idx] = '1';
        }
        n_raw >>= 1;
        idx -= 1;
    }
    return result;
}

// This should be replaced with gtest asserts or something; see disclaimer.
void TestTask1() {
    std::cout << ToBinaryNoTrailing(1) << std::endl;
    std::cout << ToBinaryNoTrailing(10) << std::endl;
    std::cout << ToBinaryNoTrailing(-1) << std::endl;
    std::cout << ToBinaryNoTrailing(INT_MAX) << std::endl;
    std::cout << ToBinaryNoTrailing(INT_MIN) << std::endl;
}
}  // namespace task_1

namespace task_2 {
void RemoveDups(char* str) {
    if (str == NULL || str[0] == '\0') return;
    char* read_ptr = str;
    char* write_ptr = str;

    do {
        if (*++read_ptr != *write_ptr) {
            if (++write_ptr != read_ptr) {
                *write_ptr = *read_ptr;
            }
        }
    } while (*read_ptr != '\0');
}

void TestTask2() {
    char data[] = "AAA BBB   AAA";
    RemoveDups(data);
    printf("%s\n", data);
}
}  // namespace task_2

namespace task_3 {
struct ListNode {
    ListNode* prev = nullptr;
    ListNode* next = nullptr;
    ListNode* rand = nullptr;

    std::string data;
};

class List{
public:
    void Serialize(FILE* file);
    void Deserialize(FILE* file);
    void PushBack(ListNode* node);
    void Print();

    virtual ~List() {
        delete[] _storage;
    }

private:
    // These are yet not implemented methods required to guarantee
    // (de)serialization would be 100% safe.
    // void Validate();
    // void Clear();

    ListNode* head = nullptr;
    ListNode* tail = nullptr;
    int count = 0;

    ListNode* _storage = nullptr;
};

// RAII is badly violated in this task, so I assume
// we can both initialize List by calling Deserialize()
// and by manually adding some elements (in which case we
// are the ones responsible for memory (de)allocation).
void List::PushBack(ListNode* node) {
    if (!head) {
        head = node;
        tail = node;
    } else {
        tail->next = node;
        auto temp = tail;
        tail = node;
        tail->prev = temp;
    }
    count += 1;
}

// I'd prefer ToString() of sorts, but I needed a quick check
void List::Print() {
    ListNode* ptr = head;
    while (ptr) {
        std::cout << ptr->data << std::endl;
        ptr = ptr->next;
    }
}

// This method assumes List's data correctness and is unsafe otherwise.
// Validate() implementation is required.
void List::Serialize(FILE* file) {
    if (file == NULL) {
        throw std::runtime_error(
                "Failed to serialize List: incorrect FILE* object");
    }
    if (count == 0) return;

    // Learn to distinguish nodes
    std::map<ListNode*, size_t> node_ids;
    ListNode* node_ptr = head;
    for (size_t node_idx = 0; node_idx < count; ++node_idx) {
        node_ids.emplace(node_ptr, node_idx);
        node_ptr = node_ptr->next;
    }

    // Prepare random links' data
    std::vector<int> buffer(count, -1);
    node_ptr = head;
    for (size_t node_idx = 0; node_idx < count; ++node_idx) {
        if (node_ptr->rand != nullptr) {
            buffer[node_idx] = node_ids.find(node_ptr->rand)->second;
        }
        node_ptr = node_ptr->next;
    }

    // Serialize all data into file
    size_t fwrite_counter = 0;  // Used later to check write correctness
    fwrite_counter += std::fwrite(&count, sizeof(count), 1, file); 
    fwrite_counter += std::fwrite(
            buffer.data(), buffer.size() * sizeof(int), 1, file);
    node_ptr = head;  // Ugly iterator
    for (size_t node_idx = 0; node_idx < count; ++node_idx) {
        size_t string_len = node_ptr->data.size() + 1;  // '\0' included
        fwrite_counter += std::fwrite(&string_len, sizeof(string_len), 1, file);
        fwrite_counter += std::fwrite(
                node_ptr->data.c_str(), string_len * sizeof(char), 1, file);
        node_ptr = node_ptr->next;
    }

    if (fwrite_counter != 2 + 2 * count) {
        throw std::runtime_error(
                "fwrite() failed somewhere during serialization");
    }
}

void List::Deserialize(FILE* file) {
    if (file == NULL) {
        throw std::runtime_error(
                "Failed to deserialize List: incorrect FILE* object");
    }

    // TODO: clear the object before deserialization

    size_t fread_counter = 0;  // Used later to check read correctness

    // Read count
    int count;  // Ugly copying, I wasn't sure if I can &(this->count) directly
    fread_counter += std::fread(&count, sizeof(count), 1, file);
    this->count = count;
    if (count <= 0) return;  // TODO: add corrupted file's handling: count < 0?

    // Read random links to nodes
    int* rand_links = new int[count];
    fread_counter += std::fread(rand_links, count * sizeof(*rand_links), 1, file);

    // Read strings
    // We'll do the dirty hack: use a char* buffer to read data,
    // store it into std::string through a cast,
    // temporarily push it to a std::vector<std::string>,
    // then move data to nodes.
    std::vector<std::string> strings;
    size_t string_buffer_len = 4;  // Initial buffer size
    char* string_buffer = new char[string_buffer_len];
    for (size_t node_idx = 0; node_idx < count; ++node_idx) {
        size_t string_len;
        fread_counter += std::fread(&string_len, sizeof(string_len), 1, file);
        if (string_len > string_buffer_len) {
            delete[] string_buffer;
            string_buffer_len = 2 * string_len;  // 2x to reduce future realloc
            string_buffer = new char[string_buffer_len];
        }
        fread_counter += std::fread(string_buffer, string_len, 1, file);
        strings.push_back(std::string(string_buffer));
    }

    // Restore List
    _storage = new ListNode[count];  // This is very bad, but I have no choice
    head = _storage;
    tail = _storage + count - 1;  // No breaking boundaries here, count > 0
    for (size_t node_idx = 0; node_idx < count; ++node_idx) {
        ListNode* ptr = _storage + node_idx;
        ptr->data = strings[node_idx];
        if (node_idx > 0) {
            ptr->prev = ptr - 1;
            (ptr - 1)->next = ptr;
        }
        if (rand_links[node_idx] != -1) {
            ptr->rand = _storage + rand_links[node_idx];
        }
    }

    delete[] rand_links;
    delete[] string_buffer;

    if (fread_counter != 2 + 2 * count) {
        throw std::runtime_error(
                "fread() failed somewhere during deserialization");
    }
}

void TestTask3 () {
    // Here we'll face the consequences of violating RAII in detail.

    // Create a list    
    List list;
    ListNode* nodes = new ListNode[3];
    nodes->data = "first";
    (nodes + 1)->data = "second";
    (nodes + 2)->data = "third";
    nodes->rand = nodes + 2;
    list.PushBack(nodes);
    list.PushBack(nodes + 1);
    list.PushBack(nodes + 2);

    std::cout << "Initial list nodes' data:" << std::endl;
    list.Print();

    // Write list contents to file
    FILE* output = fopen("test.bin", "wb");
    try {
        list.Serialize(output);
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    fclose(output);
    
    // Read contents back from file
    FILE* input = fopen("test.bin", "rb");
    List new_list;
    try {
        new_list.Deserialize(input);
        std::cout << "Deserialized list nodes' data:" << std::endl;
        new_list.Print();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    fclose(input);

    delete[] nodes;
}
}  // namespace task_3


int main() {
    std::cout << "========== TestTask1 output: ==========" << std::endl;
    task_1::TestTask1();
    std::cout << "========== TestTask2 output: ==========" << std::endl;
    task_2::TestTask2();
    std::cout << "========== TestTask3 output: ==========" << std::endl;
    task_3::TestTask3();
}
