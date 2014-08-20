#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#define DEBUG (1)
#if DEBUG
#define debug(M, ...) fprintf(stderr, "%s: " M, __FUNCTION__, ##__VA_ARGS__)
#else
#define debug(M, ...)
#endif

using namespace std;

enum STATUS{
    STATUS_NULL,
    STATUS_AUCTION,
    STATUS_SOLD,
    STATUS_UNSOLD
};

class Item{
    public:
    STATUS status;
    string product;
    int min_price;
    int curr_price;
    string buyer;
    string bidder;
    Item(){
        this->product = "";
        this->min_price = 0;
        this->status = STATUS_NULL;
        this->curr_price = 0;
        this->buyer = "";
        this->bidder = "";
    }
    Item(string product, int min_price){
        this->product = product;
        this->min_price = min_price;
        this->status = STATUS_NULL;
        this->curr_price = 0;
        this->buyer = "";
        this->bidder = "";
    }
    friend ostream& operator<<(ostream& os, const Item& it){
        os<<"--product: "<<it.product<<endl;
        os<<"--current status: ";
        if(it.status == STATUS_NULL){
            os<<"no auction"<<endl;
        }
        else if(it.status == STATUS_UNSOLD){
            os<<"unsold"<<endl;
            os<<"--price: "<<it.curr_price<<endl;
        }
        else if(it.status == STATUS_SOLD){
            os<<"sold"<<endl;
            os<<"--buyer: "<<it.buyer<<endl;
            os<<"--price: "<<it.curr_price<<endl;
        }
        else if(it.status == STATUS_AUCTION){
            os<<"on auction"<<endl;
            os<<"--bidder: "<<it.bidder<<endl;
            os<<"--price: "<<it.curr_price<<endl;
        }
        return os;
    }
};

class Trie{
    private:
        char c;
        vector<Trie*> chd;
        void* data;
        int _insert(string& str, int idx, void* data){
            if(idx == str.size()){
                this->data = data;
                return 1;
            }
            for(int i = 0; i < this->chd.size(); i++){
                if(this->chd[i]->c == str.at(idx)){
                    this->chd[i]->_insert(str, idx+1, data);
                    return 1;
                }
            }
            Trie* nn = new Trie(str.at(idx));
            this->chd.push_back(nn);
            nn->_insert(str, idx+1, data);
            return 1;
        }
        Trie* _find(string& str, int idx){
            if(idx == str.size()){
                return this;
            }
            for(int i = 0; i < this->chd.size(); i++){
                if(this->chd[i]->c == str.at(idx)){
                    return this->chd[i]->_find(str, idx+1);
                }
            }
            return NULL;
        }
    public:
        Trie(char c){
            data = NULL;
            this->c = c;
        }
        Trie(){
            data = NULL;
            this->c = '$';
        }
        int insert(string& str, void* data){
            _insert(str, 0, data);
        }
        void* find(string& str){
            Trie* temp = _find(str, 0);
            if(temp)
                return temp->data;
            else
                return NULL;
        }
        void* del(string& str){
            Trie* temp = _find(str, 0);
            void* tdata;
            if(temp){
                tdata = temp->data;
                temp->data = NULL;
            }
            else
                tdata = NULL;
            return tdata;
        }
        void show(int d, int f){
            if(f){
                for(int i = 0; i < d; i++){
                    printf(" ");
                }
            }
            printf("%c", this->c);
            for(int i = 0; i < this->chd.size(); i++){
                this->chd[i]->show(d+1, i);
            }
            if(this->chd.size() == 0)
                printf("\n");
        }
};
class ItemLs{
    Trie itemLs;
    public:
        int insert(string product, int min_price){
            Item* pit = new Item(product, min_price);
            itemLs.insert(pit->product, pit);
        }
        int del(string product){
            Item* pit = (Item*)itemLs.del(product);
            if(pit){
                delete pit;
            }
        }
        Item* find(string product){
            return (Item*)itemLs.find(product);
        }
};
class Auction{
    ItemLs itLs;
public:
    int insert(string product, int min_price){
        itLs.insert(product, min_price);
        debug("%s %d ok\n", product.c_str(), min_price);
    }
    int del(string product){
        itLs.del(product);
        debug("%s ok\n", product.c_str());
    }
    int start(string product){
        Item* it;
        if(it = itLs.find(product)){
            if(it->status == STATUS_UNSOLD || it->status == STATUS_NULL){
                it->curr_price = 0;
                it->status = STATUS_AUCTION;
                debug("%s ok\n", product.c_str());
                return 1;
            }
            else{
                debug("%s failed\n", product.c_str());
                return 0;
            }
        }
        else{
            debug("%s failed\n", product.c_str());
            return 0;
        }
    }
    int end(string product){
        Item* it;
        if(it = itLs.find(product)){
            if(it->status == STATUS_AUCTION){
                if(it->curr_price >= it->min_price){
                    it->status = STATUS_SOLD;
                    it->buyer = it->bidder;
                }
                else{
                    it->status = STATUS_UNSOLD;
                }
                debug("%s ok\n", product.c_str());
                return 1;
            }
            else{
                debug("%s failed\n", product.c_str());
                return 0;
            }
        }
        debug("%s failed\n", product.c_str());
        return 0;
    }
    int bid(string bidder, string product, int price){
        Item* it;
        if(it = itLs.find(product)){
            if(it->status == STATUS_AUCTION && price > it->curr_price){
                it->bidder = bidder;
                it->curr_price = price;
                debug("%s %s %d ok\n", bidder.c_str(), product.c_str(), price);
                return 1;
            }
        }
        debug("%s %s %d failed\n", bidder.c_str(), product.c_str(), price);
        return 0;
    }
    int query(string product, Item& it){
        Item* pit;
        pit = itLs.find(product);
        int result = (pit != 0);
        if(pit){
            it = *pit;
            debug("found %s\n", product.c_str());
#if DEBUG
            cout<<it;
#endif
        }
        else{
            debug("not found %s\n", product.c_str());
        }
        return result;
    }
};

int main(){
    Auction auc;
    Item itf;

    auc.insert("car", 10);
    auc.insert("card", 37);
    auc.insert("coffee", 23);

    auc.start("car");
    auc.start("coffee");
    auc.start("abc");

    auc.bid("angie", "abc", 5);

    auc.bid("angie", "car", 5);
    auc.bid("grant", "car", 13);
    auc.bid("grant", "car", 11);

    auc.bid("mary",  "coffee", 20);
    auc.bid("john", "coffee", 2121);

    auc.query("car", itf);
    auc.query("card", itf);
    auc.query("coffee", itf);
    auc.query("abc", itf);

    auc.end("car");
    auc.end("card");
    auc.end("coffee");
    auc.end("abc");

    auc.query("car", itf);
    auc.query("card", itf);
    auc.query("coffee", itf);
    auc.query("abc", itf);

    auc.del("car");
    auc.del("abc");

    auc.query("car", itf);
    auc.query("card", itf);
    auc.query("coffee", itf);
    auc.query("abc", itf);
}
