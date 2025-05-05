#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <iomanip>

using namespace std;
using namespace std::chrono;

/******************************************************************************
 * A simple Node struct                                                       *
 * ch - character value                                                       *
 * frq - frequncy of the character                                            *
 * code - the binary code produced by the huffman tree                        *
 * left - left child                                                          *
 * right - right child                                                        *
 * ****************************************************************************/
struct Node
{
    char ch;
    int frq;
    string code;
    Node *left;
    Node *right;

    Node(char c, int i) : ch(c), frq(i), left(nullptr), right(nullptr) {}
};

/******************************************************************************
 * getFrequency                                                               *
 * calculates the frequency of each character in the original message         *
 * originalMessage - the input string to analyze                              *
 * nodes - vector to store unique characters and their frequencies            *
 ******************************************************************************/
void getFrequency(string originalMessage, vector<Node *> &nodes)
{
    for (char c : originalMessage)
    {
        bool found = false;
        for (Node *node : nodes)
        {
            // if node char matches original character
            if (node->ch == c)
            {
                node->frq++;
                found = true;
            }
        }
        if (!found)
        {
            nodes.push_back(new Node(c, 1));
        }
    }
}

/******************************************************************************
 * compareFrequencies                                                         *
 * struct used to order nodes in the priority queue based on frequency        *
 * returns true if the frequency of a > b                                     *
 ******************************************************************************/
struct compareFrequencies
{
    bool operator()(Node *a, Node *b)
    {
        return a->frq > b->frq;
    }
};

/******************************************************************************
 * makeHuffmanTree                                                            *
 * builds a Huffman tree from a list of unique character nodes                *
 * nodes - vector of Node pointers with initial frequencies                   *
 * returns a pointer to the root of the constructed Huffman tree              *
 ******************************************************************************/
Node *makeHuffmanTree(vector<Node *> nodes)
{
    // initialize min-heap priority queue
    priority_queue<Node *, vector<Node *>, compareFrequencies> PQ;

    // push nodes onto the queue
    for (Node *node : nodes)
        PQ.push(node);

    // merge nodes until one root remains
    while (PQ.size() > 1)
    {
        Node *t1 = PQ.top();
        PQ.pop();
        Node *t2 = PQ.top();
        PQ.pop();

        // create new internal node with combined frequency
        Node *tNode = new Node('\0', t1->frq + t2->frq);

        tNode->left = t1;
        tNode->right = t2;

        PQ.push(tNode);
    }

    Node *root = PQ.top();

    return root;
}

/******************************************************************************
 * getHuffmanCodes                                                            *
 * traverses the Huffman tree and assigns binary codes to each character      *
 * node - current node being visited                                          *
 * currCode - the binary string built so far                                  *
 ******************************************************************************/
void getHuffmanCodes(Node *node, string currCode)
{
    // if no node
    if (!node)
        return;

    // if leaf node reached
    if (node->ch != '\0')
        node->code = currCode;

    // go left and add 0
    getHuffmanCodes(node->left, currCode + "0");
    // go right and add 1
    getHuffmanCodes(node->right, currCode + "1");
}

/******************************************************************************
 * encode                                                                     *
 * makes new string using the Huffman codes stored in the nodes               *
 * originalMessage - the original input string                                *
 * nodes - list of nodes with assigned Huffman codes                          *
 * returns a compressed binary string                                         *
 ******************************************************************************/
string encode(string originalMessage, vector<Node *> nodes)
{
    string compressedMessage;

    for (char c : originalMessage)
    {
        for (Node *node : nodes)
        {
            if (node->ch == c)
            {
                compressedMessage += node->code;
                break;
            }
        }
    }

    return compressedMessage;
}

/******************************************************************************
 * decode                                                                     *
 * decompresses a binary string using the Huffman tree                        *
 * compressedMessage - the binary string to decode                            *
 * root - the root of the Huffman tree                                        *
 * returns the original string                                                *
 ******************************************************************************/
string decode(string compressedMessage, Node *root)
{
    string result;
    Node *currNode = root;

    for (char c : compressedMessage)
    {
        if (c == '0')
            currNode = currNode->left;
        else if (c == '1')
            currNode = currNode->right;

        if (currNode->ch != '\0')
        {
            result += currNode->ch;
            currNode = root;
        }
    }

    return result;
}

/******************************************************************************
 * compareMessages                                                            *
 * compares the original and decompressed messages for accuracy               *
 * originalMessage - the original string before compression                   *
 * decompressedMessage - the string after decompression                       *
 * prints whether the decompression was successful or where a mismatch occurs *
 ******************************************************************************/
void compareMessages(string originalMessage, string decompressedMessage)
{
    cout << "\nDecompressing Message\n";

    // checks if length matches after decompression
    if (originalMessage.length() != decompressedMessage.length())
    {
        cout << "Differing Amounts of Data\n";
        return;
    }

    // checks each character against original message
    for (int i = 0; i < originalMessage.length(); i++)
    {
        if (originalMessage[i] != decompressedMessage[i])
        {
            cout << "Missmatch at index: " << i << " | Expected: " << originalMessage[i] << " | Recieved: " << decompressedMessage[i] << endl;
            return;
        }
    }

    cout << "Decompression Successful: Messages Are Identical\n";
}

int main()
{
    cout << fixed << setprecision(1);
    char choice = 'y';
    do
    {
        // Test cases came from https://corpus.canterbury.ac.nz/descriptions/
        string TEST_CASES[7] = {"artificl/aaa.txt", "artificl/alphabet.txt", "artificl/random.txt", "large/bible.txt", "large/E.coli", "large/pi.txt", "large/world192.txt"};

        // If you want to add more test cases, add the local path to this array and increment the size of the array
        // Then enter the corresponding number when prompted

        int testChoice = 0;

        cout << "\n\n0) The letter 'a', repeated 100,000 times." << endl;
        cout << "1) Enough repetitions of the alphabet to fill 100,000 characters" << endl;
        cout << "2) 100,000 characters, randomly selected from [a-z|A-Z|0-9|!| ] (alphabet size 64)" << endl;
        cout << "3) The King James version of the bible" << endl;
        cout << "4) Complete genome of the E. Coli bacterium" << endl;
        cout << "5) The first million digits of pi" << endl;
        cout << "6) The CIA world fact book" << endl;
        cout << "\nChoose a test case 0-6: ";
        cin >> testChoice;

        ifstream inFile(TEST_CASES[testChoice]);
        string originalMessage;
        vector<Node *> nodes;
        string line;

        while (getline(inFile, line))
            originalMessage += line + '\n';

        int originalSize = originalMessage.length() * 8;

        cout << "\n-----------------------------------------------------------\n";
        cout << "\nOriginal File Size: " << originalSize << " bits\n";

        auto start = high_resolution_clock::now();
        getFrequency(originalMessage, nodes);
        Node *root = makeHuffmanTree(nodes);
        getHuffmanCodes(root, "");
        string compressedMessage = encode(originalMessage, nodes);
        string decompressedMessage = decode(compressedMessage, root);
        auto stop = high_resolution_clock::now();

        auto duration = duration_cast<microseconds>(stop - start);
        int compressedSize = compressedMessage.length();
        double percentDiff = (((double)originalSize - compressedSize) / originalSize) * 100;

        cout << "Compressed Size: " << compressedSize << " bits\n";
        cout << "Total Unique Characters: " << nodes.size() << endl;
        cout << "Percent Difference: " << percentDiff << "% improvement\n";
        compareMessages(originalMessage, decompressedMessage);
        cout << "\nTotal Time: " << duration.count() << " microseconds" << endl;
        cout << "\n-----------------------------------------------------------\n";

        cout << "\nWould you like to test another file? (y/n): ";
        cin >> choice;
        tolower(choice);

    } while (choice == 'y');

    return 0;
}