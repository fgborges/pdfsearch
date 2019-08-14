#include <poppler-document.h>
#include <poppler-page.h>
#include <iostream>
#include <memory>
#include <regex>
#include <list>

struct Item {
    std::string path;
    int index;
    std::string line;
};

std::list<Item> pdfsearch(std::string query, std::string path) {
    std::list<Item> items;
    poppler::ustring uquery(poppler::ustring::from_latin1(query));
    std::unique_ptr<poppler::document> document(poppler::document::load_from_file(path, "", ""));
    for(int index = 0; index < document->pages(); index++) {
        std::unique_ptr<poppler::page> page(document->create_page(index));
        poppler::rectf page_rect(page->page_rect());
        poppler::rectf rect(0, 0, 0, 0);
        if(page->search(uquery, rect, poppler::page::search_direction_enum::search_from_top, poppler::case_sensitivity_enum::case_insensitive)){
            do {
                poppler::rectf line_rect(rect);
                line_rect.set_left(page_rect.left());
                line_rect.set_right(page_rect.right());
                poppler::ustring text(page->text(line_rect));
                std::string latin1_text(regex_replace(text.to_latin1(), std::regex("[^\\x00-\\x7F]|\\s+"), " "));
                items.push_back(Item{path, index, latin1_text});
            } while(page->search(uquery, rect, poppler::page::search_direction_enum::search_next_result, poppler::case_sensitivity_enum::case_insensitive));
        }
    }
    return items;
}

int main(int argc, char** argv) {
    for(int i = 2; i < argc; i++){
        for(Item item : pdfsearch(std::string(argv[1]), std::string(argv[i]))) {
            std::cout << item.path << ":" << item.index << ":" << item.line << std::endl;
        }
    }
}
