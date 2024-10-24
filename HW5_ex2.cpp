#include <iostream>
#include <fstream>
#include <string>

class TextDocument {
private:
    std::string text;
    std::string name;

public:
    TextDocument(const std::string& name) : name(name), text("") {}

    std::string getText() const { return text; }
    void setText(const std::string& newText) { text = newText; }
    std::string getName() const { return name; }
};

class TextEditor {
private:
    TextDocument* currentDocument;

public:
    void createDocument() {
        currentDocument = new TextDocument("Новый документ");
    }

    void openDocument(const std::string& filename) {
        std::ifstream file(filename);
        std::string content;
        std::getline(file, content, '\0'); 
        currentDocument = new TextDocument(filename);
        currentDocument->setText(content);
    }

    void saveDocument() {
        std::ofstream file(currentDocument->getName());
        file << currentDocument->getText();
    }

    void saveDocumentAs(const std::string& filename) {
        currentDocument->setText(filename);
        saveDocument();
    }

    void printDocument() {
        std::cout << currentDocument->getText() << std::endl; 
    }

    void closeDocument() {
        delete currentDocument;
        currentDocument = nullptr;
    }
};

int main() {
    TextEditor editor;
    editor.createDocument();
    // ...
    return 0;
}