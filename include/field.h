#pragma once

#include <string>
#include "art.h"
#include "option.h"
#include "string_utils.h"

namespace field_types {
    static const std::string STRING = "string";
    static const std::string INT32 = "int32";
    static const std::string INT64 = "int64";
    static const std::string FLOAT = "float";
    static const std::string BOOL = "bool";
    static const std::string STRING_ARRAY = "string[]";
    static const std::string INT32_ARRAY = "int32[]";
    static const std::string INT64_ARRAY = "int64[]";
    static const std::string FLOAT_ARRAY = "float[]";
    static const std::string BOOL_ARRAY = "bool[]";
}

namespace fields {
    static const std::string name = "name";
    static const std::string type = "type";
    static const std::string facet = "facet";
}

struct field {
    std::string name;
    std::string type;
    bool facet;

    field(const std::string & name, const std::string & type, const bool & facet): name(name), type(type), facet(facet) {

    }

    bool is_single_integer() const {
        return (type == field_types::INT32 || type == field_types::INT64);
    }

    bool is_single_float() const {
        return (type == field_types::FLOAT);
    }

    bool is_single_bool() const {
        return (type == field_types::BOOL);
    }

    bool is_integer() const {
        return (type == field_types::INT32 || type == field_types::INT32_ARRAY ||
               type == field_types::INT64 || type == field_types::INT64_ARRAY);
    }

    bool is_float() const {
        return (type == field_types::FLOAT || type == field_types::FLOAT_ARRAY);
    }

    bool is_bool() const {
        return (type == field_types::BOOL || type == field_types::BOOL_ARRAY);
    }

    bool is_string() const {
        return (type == field_types::STRING || type == field_types::STRING_ARRAY);
    }

    bool is_facet() const {
        return facet;
    }
};

struct filter {
    std::string field_name;
    std::vector<std::string> values;
    NUM_COMPARATOR compare_operator;

    static Option<NUM_COMPARATOR> extract_num_comparator(const std::string & comp_and_value) {
        if(StringUtils::is_integer(comp_and_value)) {
            return Option<NUM_COMPARATOR>(EQUALS);
        }

        // the ordering is important - we have to compare 2-letter operators first
        if(comp_and_value.compare(0, 2, "<=") == 0) {
            return Option<NUM_COMPARATOR>(LESS_THAN_EQUALS);
        }

        if(comp_and_value.compare(0, 2, ">=") == 0) {
            return Option<NUM_COMPARATOR>(GREATER_THAN_EQUALS);
        }

        if(comp_and_value.compare(0, 1, "<") == 0) {
            return Option<NUM_COMPARATOR>(LESS_THAN);
        }

        if(comp_and_value.compare(0, 1, ">") == 0) {
            return Option<NUM_COMPARATOR>(GREATER_THAN);
        }

        return Option<NUM_COMPARATOR>(400, "Numerical field has an invalid comparator.");
    }
};

namespace sort_field_const {
    static const std::string name = "name";
    static const std::string order = "order";
    static const std::string asc = "ASC";
    static const std::string desc = "DESC";
}

struct sort_by {
    std::string name;
    std::string order;

    sort_by(const std::string & name, const std::string & order): name(name), order(order) {

    }

    sort_by& operator=(sort_by other) {
        name = other.name;
        order = other.order;
        return *this;
    }
};

struct facet {
    const std::string field_name;
    std::map<std::string, size_t> result_map;

    facet(const std::string field_name): field_name(field_name) {

    }
};

struct facet_value {
    // use string to int mapping for saving memory
    spp::sparse_hash_map<std::string, uint32_t> value_index;
    spp::sparse_hash_map<uint32_t, std::string> index_value;

    spp::sparse_hash_map<uint32_t, std::vector<uint32_t>> doc_values;

    uint32_t get_value_index(const std::string & value) {
        if(value_index.count(value) != 0) {
            return value_index[value];
        }

        uint32_t new_index = value_index.size();
        value_index.emplace(value, new_index);
        index_value.emplace(new_index, value);
        return new_index;
    }

    void index_values(uint32_t doc_seq_id, const std::vector<std::string> & values) {
        std::vector<uint32_t> value_vec(values.size());
        for(size_t i = 0; i < values.size(); i++) {
            value_vec[i] = get_value_index(values[i]);
        }
        doc_values.emplace(doc_seq_id, value_vec);
    }
};