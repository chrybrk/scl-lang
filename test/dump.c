void gen_let(gen_T* gen, ast_T* node)
{
    printf("%s\n", node->token->tokened_line), exit(0);

    struct function_writeable* fnc = gen->functions->buffer[gen->functions->index - 1];
    struct hash_pair* value = hashmap_find(gen->hashmap, node->token->value);

    if (!value)
    {
        char data_type_size = get_data_type_size(node->data_type); // TODO: check for -1
        char* data_type_value = get_data_type_value(data_type_size); // TODO: check for NULL

        gen->last_stack_index += data_type_size;
        ast_T* next_node = node->node;

        if (next_node->token->token_type == T_IDENT && next_node->ast_type == AST_EXPR)
        {
            struct stack_variable* r_var = gen->vars->buffer[hashmap_find(gen->hashmap, next_node->token->value)->value];
            if (!r_var) init_error_with_token(next_node->token, E_FAILED, writef("variable not defined, '%s'.\n", next_node->token->value));
            char data_type_size_for_rop = get_data_type_size(r_var->data_type);
            if ((int)data_type_size < (int)data_type_size_for_rop)
                printf("[WARN]: `%s -> %s`, moving higher size variable data to lower size variable, might result in loss of data.\n", r_var->identifier, node->token->value);

            char* r = get_register(data_type_size, RT_reg);
            char* gr = get_register(data_type_size, RT_gen);
            gen_statement(gen, node->node);
            fnc->content = alloc_str(writef("\tmov %s, %s\n", r, gr), fnc->content);
            fnc->content = alloc_str(writef("\tmov %s [rbp - %ld], %s\n", data_type_value, gen->last_stack_index, r), fnc->content);
            free_register(RT_reg);
        }
        else
        {
            gen_statement(gen, node->node);
            char* gr = get_register(data_type_size, RT_gen);
            fnc->content = alloc_str(writef("\tmov %s [rbp - %ld], %s\n", data_type_value, gen->last_stack_index, gr), fnc->content);
        }

        struct stack_variable* var = calloc(1, sizeof(struct stack_variable));
        var->data_type = node->data_type;
        var->identifier = node->token->value;
        var->index = gen->last_stack_index;

        // FIXME: redundant use case of `gen->size_of_stack`, when instead we can just use, `array->index`.

        hashmap_insert(gen->hashmap, node->token->value, gen->size_of_stack);
        array_push(gen->vars, var);
        gen->size_of_stack++;
    }
    else printf("%s\n", node->token->tokened_line), exit(1);

}
