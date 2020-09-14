#include <iostream>
#include <cmath>

char *encode( char *plaintext, unsigned long key);
char *decode( char *ciphertext, unsigned long key);


#ifndef MARMOSET_TESTING
int main(){
    char str0[]{"Hello world!"};
    char *ciphertext{ encode( str0, 123 )};
    std::cout << ciphertext << std::endl;
    char *plaintext{ decode( ciphertext, 123 )};
    std::cout << plaintext << std::endl;
    return 0;
}
#endif

char *encode(char *plaintext,unsigned long key){
    unsigned char S[256]{};
    for (std::size_t c{0}; c<256; c++) {
        S[c]=c;
    }
    int i{0};
    int j{0};
    int k{0};
    int temp{};
    //scrambling s--------------------------
    for (std::size_t c{0}; c<256; c++) {
        k = i%64;
        j = ( j + S[i] + ((key>>k) & 1) ) % 256;
        temp=S[i];
        S[i]=S[j];
        S[j]=temp;
        i=((i+1) % 256);
    }
    //finding length of word
    int text_length{0};
    int check_char{ plaintext[0] };
    while(check_char != '\0'){
        text_length++;
        check_char=plaintext[text_length];
    }
    //-----------------------------------
    //making length multiple of 4
    int new_length=0;
    if((text_length%4)!=0){
        new_length=text_length+(4 -(text_length%4));
    }
    else{
        new_length=text_length;
    }
    char *new_plaintext=new char[(int)(new_length+1)];
    new_plaintext[new_length]='\0';
    for(int c=0;c < new_length;c++){
        new_plaintext[c]='\0';
    }
    for(int c=0;c<text_length;c++){
        new_plaintext[c]=plaintext[c];
    }
    //----------------------------------
    //finding R------------------------
    for(std::size_t c{0}; c < new_length ; c++){
        i=(i+1) % 256;
        j= (j+S[i]) % 256;
        temp=S[i];
        S[i]=S[j];
        S[j]=temp;
        int r = (S[i] + S[j]) % 256;
        int R = S[r];
        // xor each char with R----------
        new_plaintext[c] = new_plaintext[c] ^ R;
    }
    //ascii armour
    int groups=new_length/4;
    char *encoded = new char[(int)(5*(std::ceil(new_length/4)) + 1)];
    encoded[(int)(5*((std::ceil(new_length/4))))]='\0';
    unsigned char convert[4]{};
    unsigned int thirty2_bit{0};
    char base85[5]{};
    for(std::size_t g{0};g<groups;g++){
        
        thirty2_bit=0;
        for(std::size_t j{0}; j < 4; j++){
            convert[j]=new_plaintext[4*g + j];
        }
        thirty2_bit=(convert[3])|(convert[2] << 8)|(convert[1] << 16)|(convert[0] << 24);
        //convert to base 85
        for(std::size_t j{0};j<5;j++){
            base85[4-j]=(thirty2_bit % 85)+'!';
            thirty2_bit=thirty2_bit/85;
        }
        for(std::size_t z{0}; z < 5;z++){
            encoded[(5*g)+z]=base85[z];
        }
    }
    return encoded;
}

//decode--------------------------------------------------------------------------------------------------------------
char *decode( char *ciphertext, unsigned long key){
        //finding length of ciphertext
    int ciphertext_length{0};
    int check_char{ ciphertext[0] };
    while(check_char != '\0'){
        ciphertext_length++;
        check_char=ciphertext[ciphertext_length];
    }
        //converting base 85 back to original charaters(oh lord)
    int groups = ciphertext_length/5;
    char *anti_aa = new char[(int)(4 * (ciphertext_length / 5) + 1)]{};
    anti_aa[4 * (ciphertext_length / 5)]='\0';
    char middle_man[4]{};
    unsigned int decimal_sum{0};
    
    for(std::size_t c{0};c < groups; c++){
        decimal_sum=0;
        
        for(std::size_t a{0}; a<5; a++){
            decimal_sum = decimal_sum +  ( ( ciphertext[5*c + a] - '!') * pow(85,4-a) );
        }
        
        middle_man[0] = ( decimal_sum >> 24 ) & 255;
        middle_man[1] = ( decimal_sum >> 16 ) & 255;
        middle_man[2] = ( decimal_sum >> 8 ) & 255;
        middle_man[3] =  decimal_sum  & 255;
        
        for(std::size_t a{0}; a<4; a++){
            anti_aa[4*c + a] = middle_man[a];
        }
    }
    //----------------------------------------
    
    unsigned char S[256]{};
    for (std::size_t c{0}; c<256; c++) {
        S[c]=c;
    }
    int i{0};
    int j{0};
    int k{0};
    int temp{};
    //scrambling s--------------------------
    for (std::size_t c{0}; c<256; c++) {
        k = i%64;
        j = ( j + S[i] + ((key>>k) & 1) ) % 256;
        temp=S[i];
        S[i]=S[j];
        S[j]=temp;
        i=((i+1) % 256);
    }
    //----------------------
    int anti_aa_length = 4 * (ciphertext_length / 5);
    //finding R------------------------
    for(std::size_t c{0}; c < anti_aa_length ; c++){
        i=(i+1) % 256;
        j= (j+S[i]) % 256;
        temp=S[i];
        S[i]=S[j];
        S[j]=temp;
        int r = (S[i] + S[j]) % 256;
        int R = S[r];
        // xor each char with R----------
        anti_aa[c] = anti_aa[c] ^ R;
    }
    //-------------------------
    return anti_aa;
}
