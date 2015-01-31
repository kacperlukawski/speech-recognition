#include "HMM.h"

speech::spelling::HMM::HMM() {
}

//
// @todo implement loading the model
//
speech::spelling::HMM::HMM(std::istream &in) {

}

void speech::spelling::HMM::fit(int *phonemes, string spelling) {

}

string speech::spelling::HMM::predict(int *phonemes) {
    return std::string("sg");
}

//
// @todo implement the serialization
//
void speech::spelling::HMM::serialize(std::ostream &out) const {
    uint32_t typeIdentifier = TYPE_IDENTIFIER;
    out.write((char const *) &typeIdentifier, sizeof(typeIdentifier));

}
