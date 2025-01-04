#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl.H>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class HTMLElement {
public:
    std::string tag;
    std::string content;
    std::vector<HTMLElement*> children;

    HTMLElement(std::string tag, std::string content = "")
        : tag(tag), content(content) {}

    void add_child(HTMLElement* child) {
        children.push_back(child);
    }
};
class HTML {
public:
    HTMLElement* root = nullptr;
    std::vector<std::string> stack;
    std::vector<std::vector<std::string>> tkp;
    void tok(std::string line);
    void par();
    void print_tree(HTMLElement* node, int depth);
    HTML() {
        root = new HTMLElement("root");  
    }
    ~HTML() {
        delete root;  
    }
};

void HTML::tok(std::string line) {
    std::vector<std::string> tk;
    int i = 0, j = 0;
    while (i <= line.length()) {
        if (i + 5 <= line.length() && line.substr(i, 5) == "<div>"){
            if (j < i) tk.push_back(line.substr(j, i - j));
            tk.push_back("<div>");
            i += 5;
            j = i;
        } else if (i + 6 <= line.length() && line.substr(i, 6) == "</div>"){
            if (j < i) tk.push_back(line.substr(j, i - j));
            tk.push_back("</div>");
            i += 5;
            j = i;
        } else if (i + 3 <= line.length() && line.substr(i, 3) == "<p>") {
            if (j < i) tk.push_back(line.substr(j, i - j));
            tk.push_back("<p>");
            i += 3;
            j = i;
        } else if (i + 4 <= line.length() && line.substr(i, 4) == "</p>") {
            if (j < i) tk.push_back(line.substr(j, i - j));
            tk.push_back("</p>");
            i += 4;
            j = i;
        } else if (line[i] == '<') {
            if (j < i) tk.push_back(line.substr(j, i - j));
            j = i;
        }
        i++;
    }
    tkp.push_back(tk);
}

void HTML::par() {
    std::vector<HTMLElement*> elementStack;
    elementStack.push_back(root);

    for (const auto& tokenGroup : tkp) {
        for (const auto& token : tokenGroup) {
            if (token[0] == '<' && token[1] != '/') {  
                std::string tagName = token.substr(1, token.length() - 2);
                HTMLElement* newElement = new HTMLElement(tagName);
                elementStack.back()->add_child(newElement);
                elementStack.push_back(newElement);
            } else if (token[0] == '<' && token[1] == '/') {  
                elementStack.pop_back();
            } else {  
                elementStack.back()->content += token;
            }
        }
    }
}

void HTML::print_tree(HTMLElement* node, int depth) {
    
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";  
    }
    std::cout << "<" << node->tag << ">" << std::endl;

    if (!node->content.empty()) {
        for (int i = 0; i < depth + 1; ++i) {
            std::cout << "  ";  
        }
        std::cout << node->content << std::endl;
    }

    
    for (auto child : node->children) {
        print_tree(child, depth + 1);
    }

    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";  
    }
    std::cout << "</" << node->tag << ">" << std::endl;
}

void add_text(Fl_Window* window, const char* text) {
    Fl_Text_Display* text_display = (Fl_Text_Display*)window->child(0); 

    Fl_Text_Buffer* buffer = text_display->buffer();
    
    buffer->append(text);  
    buffer->append("\n");  
    text_display->show_insert_position();
}
void run_html(Fl_Window* window, HTMLElement* node) {
    if (node->tag == "p") {
        add_text(window, node->content.c_str());
    }

    for (auto child : node->children) {
        run_html(window, child);
    }
}
int main() {
    Fl_Window window(800,800, "FLTK Window");
    Fl_Text_Buffer *buffer = new Fl_Text_Buffer();
    Fl_Text_Display *text_display = new Fl_Text_Display(10, 10, 800,800);
    text_display->buffer(buffer);  
    
    HTML html;
    std::ifstream file("index.html");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) { 
        html.tok(line);
    }
    html.par();
    file.close();
    run_html(&window, html.root);

    window.end();
    window.show();
    return Fl::run();
}
