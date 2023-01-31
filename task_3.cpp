#include <cstdio>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>


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
    void Validate();

    ListNode* head = nullptr;
    ListNode* tail = nullptr;
    int count = 0;

    ListNode* _storage = nullptr;
};

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

void List::Print() {
    ListNode* ptr = head;
    while (ptr) {
        std::cout << ptr->data << std::endl;
        ptr = ptr->next;
    }
}

void List::Validate() {}

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
    size_t byte_counter = 0;
    byte_counter += std::fwrite(&count, sizeof(count), 1, file);
    byte_counter += std::fwrite(buffer.data(), buffer.size() * sizeof(int), 1, file);
    node_ptr = head;
    for (size_t node_idx = 0; node_idx < count; ++node_idx) {
        size_t string_len = node_ptr->data.size() + 1;
        byte_counter += std::fwrite(&string_len, sizeof(string_len), 1, file);
        byte_counter += std::fwrite(node_ptr->data.c_str(), string_len * sizeof(char), 1, file);
        node_ptr = node_ptr->next;
    }
}


void List::Deserialize(FILE* file) {
    if (file == NULL) {
        throw std::runtime_error(
                "Failed to deserialize List: incorrect FILE* object");
    }

    // TODO: clear the object before deserialization

    size_t fread_counter = 0;

    // Read count
    int count;
    fread_counter += std::fread(&count, sizeof(count), 1, file);
    this->count = count;
    if (count <= 0) return;  // TODO: add corrupted file's handling: count < 0?

    // Read links
    int* rand_links = new int[count];
    fread_counter += std::fread(rand_links, count * sizeof(*rand_links), 1, file);

    // Read strings
    std::vector<std::string> strings;
    size_t string_buffer_len = 4;
    char* string_buffer = new char[string_buffer_len];
    for (size_t node_idx = 0; node_idx < count; ++node_idx) {
        size_t string_len;
        fread_counter += std::fread(&string_len, sizeof(string_len), 1, file);
        if (string_len > string_buffer_len) {
            delete[] string_buffer;
            string_buffer_len = 2 * string_len;
            string_buffer = new char[string_buffer_len];
        }
        fread_counter += std::fread(string_buffer, string_len, 1, file);
        strings.push_back(std::string(string_buffer));
    }

    // Restore List
    _storage = new ListNode[count];
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
}


int main () {
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

    list.Print();

    // Write list contents to file
    FILE* output = fopen("test.bin", "wb");
    list.Serialize(output);
    fclose(output);

    // Read contents back from file
    FILE* input = fopen("test.bin", "rb");
    List new_list;
    new_list.Deserialize(input);
    new_list.Print();
    fclose(input);

    delete[] nodes;
    return 0;
}
