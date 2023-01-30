#include <cstdio>
#include <fstream>
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
    void Validate();

private:
    ListNode* head = nullptr;
    ListNode* tail = nullptr;
    int count = 0;
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
}

// void List::Validate() {



//     std::map<ListNode*, size_t> ids;
//     ListNode* current_ptr = head;
//     ListNode* prev_ptr = nullptr;
//     size_t current_node_idx;

//     // 
//     while (current_ptr) {
//         // List integrity checks
//         if (current_ptr->prev != prev_ptr) {
//             throw std::runtime_error(
//                     "Incorrect prev pointer in a node");
//         }
//         if (ids.find(current_ptr) != ids.end()) {
//             throw std::runtime_error(
//                     "Found a cycle in list's graph");
//         }

//         ids.emplace(current_ptr, current_node_idx);
        
//         // Move on to the next node
//         prev_ptr = current_ptr;
//         current_ptr = current_ptr->next;
//         current_node_idx++;
//     } while (current_ptr);
//     if (current_node_idx != count) {
//         throw std::runtime_error("Number of nodes does not match count");
//     }
// }

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
    }

    // Serialize all data into file
    std::fwrite(&count, sizeof(count), 1, file);
    std::fwrite(buffer.data(), buffer.size(), 1, file);
    node_ptr = head;
    for (size_t node_idx = 0; node_idx < count; ++node_idx) {
        auto string_len = node_ptr->data.size() + 1;
        std::fwrite(&string_len, sizeof(string_len), 1, file);
        std::fwrite(node_ptr->data.c_str(), string_len, 1, file);
        node_ptr = node_ptr->next;
    }
}

void List::Deserialize(FILE* file) {
    if (file == NULL) {
        throw std::runtime_error(
                "Failed to deserialize List: incorrect FILE* object");
    }

    int count;
    std::fread(&count, sizeof(count), 1, file);
    
    int* rand_links = new int[count];
    std::fread(&rand_links, count * sizeof(*rand_links), 1, file);
}

int main () {
    // FILE* output = fopen("./test.bin", "wb");
    // if (output == NULL) {
    //     std::cerr << "File not opened" << std::endl;
    //     return -1;
    // }
    // const std::string out = "Hello";
    // int count = out.size() + 1;
    // if (std::fwrite(&count, sizeof(count), 1, output) != 1) {
    //     std::cout << "Failed to write properly" << std::endl;
    //     return -1;
    // }
    // if (std::fwrite(out.c_str(), count, 1, output) != 1) {
    //     std::cout << "Failed to write properly" << std::endl;
    //     return -1;
    // }
    // // std::fwrite(out.c_str(), out.size(), 1, output);
    // fclose(output);


    // FILE* input = fopen("./test.bin", "rb");
    // if (input == NULL) {
    //     std::cerr << "File not opened" << std::endl;
    //     return -1;
    // }
    // int count_new;
    // if (fread(&count_new, sizeof(count_new), 1, input) != 1) {
    //     std::cerr << "Failed to read properly" << std::endl;
    //     return -1;
    // }
    // char* string_buffer = new char[count_new];
    // if (fread(string_buffer, count_new * sizeof(*string_buffer), 1, input) != 1) {
    //     std::cerr << "Failed to read properly" << std::endl;
    //     return -1;
    // }
    // std::cout << count_new << std::endl;

    // string_buffer[count_new - 1] = '\0';
    // std::string new_string(string_buffer);

    // std::cout << new_string << std::endl;

    // delete[] string_buffer;

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

    // Write list contents to file
    FILE* output = fopen("test.bin", "wb");

    //

    delete[] nodes;
    return 0;
}