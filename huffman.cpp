#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <iomanip>

using namespace std;
using namespace std::chrono;

struct Node
{
    char ch;
    int frq;
    string code;
    Node *left;
    Node *right;

    Node(char c, int i) : ch(c), frq(i), left(nullptr), right(nullptr) {}
};

void getFrequency(string originalMessage, vector<Node *> &nodes)
{

    for (char c : originalMessage)
    {
        bool found = false;
        for (Node *node : nodes)
        {
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

struct compareFrequencies
{
    bool operator()(Node *a, Node *b)
    {
        return a->frq > b->frq;
    }
};

Node *makeHuffmanTree(vector<Node *> nodes)
{
    priority_queue<Node *, vector<Node *>, compareFrequencies> PQ;

    for (Node *node : nodes)
        PQ.push(node);

    while (PQ.size() > 1)
    {
        Node *t1 = PQ.top();
        PQ.pop();
        Node *t2 = PQ.top();
        PQ.pop();

        Node *tNode = new Node('\0', t1->frq + t2->frq);

        tNode->left = t1;
        tNode->right = t2;

        PQ.push(tNode);
    }

    Node *root = PQ.top();

    return root;
}

void getHuffmanCodes(Node *node, string currCode)
{
    if (!node)
        return;

    if (node->ch != '\0')
        node->code = currCode;

    getHuffmanCodes(node->left, currCode + "0");
    getHuffmanCodes(node->right, currCode + "1");
}

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

void compareMessages(string originalMessage, string decompressedMessage)
{
    cout << "\nDecompressing Message\n";
    if (originalMessage.length() != decompressedMessage.length())
    {
        cout << "Differing Amounts of Data\n";
        return;
    }

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
        string TEST_CASES[7] = {"artificl/aaa.txt", "artificl/alphabet.txt", "artificl/random.txt", "large/bible.txt", "large/E.coli", "large/pi.txt", "large/world192.txt"};
        int testChoice = 0;

        // Test cases came from https://corpus.canterbury.ac.nz/descriptions/
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