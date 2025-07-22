/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

namespace apl {

template <class S>
class YamlGenerator {
    using Str = S;
    using Ch = typename S::value_type;
    using Node = impl::YamlNode<S>;
    using NodeA = const Node&;

private:
    Str out_;
    String currentIndentString_, currentSeqIndentString_;
    int currentIndent_{};


public:
    int indentSpaces{2};

    Str& generate(const impl::Yaml<S>& yaml) {

        currentSeqIndentString_.assign(indentSpaces - 1, ' ');

        out_.clear();

        auto numDocs = yaml.documents.size();

        for (auto& doc: yaml.documents) {
            auto hasDoc = numDocs > 1 || !doc.tag.isNone();

            currentIndent_ = 0;

            if (hasDoc) {
                if (doc.tag.isNone())
                    outn("---");
                else
                    outn("--- %s%s", doc.tag.isBuiltIn() ? "!!" : "!", doc.tag.name);
            }


            for (auto& node: doc) {
                outNode(doc, node, true);
            }

            if (hasDoc)
                outn("...");
        }



        return out_;
    }

private:
    void outNode(NodeA parent, NodeA node, bool doIndent) {
        if (doIndent) {
            outIndent();
        }

        auto needIndent = true;

        if (parent.isSeq()) {
            out('-');
            out_ += currentSeqIndentString_;
            needIndent = false;
        }

        if (outNodeValue(node)) {
            nl();
            return;
        }

        for (auto& child: node) {
            //if (needIndent)
            indent();
            //else
            //    needIndent = true;

            outNode(node, child, needIndent);
            unindent();
            needIndent = true;
        }

        if (parent.isSeq()) {
            //unindent();
        }
    }

    auto outNodeKey(NodeA node) {
        if (node.key.empty())
            return false;

        if (node.tag.isNone()) {
            out(node.key);
        } else {
            auto& tag = node.tag;
            auto tagSymbol = tag.isBuiltIn() ? "!!" : "!";
            out("%s%s %s", tagSymbol, tag.name, node.key);
        }

        out(':');
        return true;
    }

    auto outNodeValue(NodeA node) {
        if (outScalar(node))
            return true;

        auto hasKey = outNodeKey(node);

        if (node.isFlowSeq()) {
            auto allScalar = true;
            for (auto& child: node) {
                if (!child.isScalar()) {
                    allScalar = false;
                    break;
                }
            }

            if (allScalar) {
                if (hasKey)
                    out(' ');

                out('[');
                auto first = true;
                for (auto& child: node) {
                    if (first)
                        first = false;
                    else {
                        out(',');
                        out(' ');
                    }
                    outScalar(child);
                }
                out(']');
                return true;
            }
        }

        if (node.isFlowMap()) {

            auto allScalar = true;
            for (auto& child: node) {
                if (!child.isScalar()) {
                    allScalar = false;
                    break;
                }
            }

            if (allScalar) {
                if (hasKey)
                    out(' ');

                out('{');
                auto first = true;
                for (auto& child: node) {
                    if (first)
                        first = false;
                    else {
                        out(',');
                        out(' ');
                    }

                    //outNodeKey(child);
                    outScalar(child);
                }
                out('}');
                return true;
            }
        }

        if (hasKey)
            nl();

        return false;
    }

    auto outScalar(NodeA node) {
        if (node.isNull()) {
            if (outNodeKey(node))
                out(' ');
            out("null");
            return true;
        }

        if (node.isScalar()) {
            if (outNodeKey(node))
                out(' ');
            out("%s", node.value);
            return true;
        }

        return false;
    }


    void outIndent() {
        out_ += currentIndentString_;
    }

    void indent() {
        currentIndent_++;
        updateIndentStrings();

        //for (int i = 0; i < currentIndent_; ++i) {

    }

    void unindent() {
        if (currentIndent_ > 0) {
            currentIndent_--;
            updateIndentStrings();
        }
    }

    void updateIndentStrings() {
        currentIndentString_.assign(currentIndent_ * indentSpaces, ' ');
    }

    void out(Ch ch) {
        out_ += ch;
    }


    void out(StrView s) {
        out_ += s;
    }

    void outn(StrView s) {
        out(s);
        nl();
    }

    void nl() {
        out_ += '\n';
    }

    template <class ...Args>
    void out(StrView fmts, Args&&... args)  {
        out(fmt(fmts, std::forward<Args>(args)...));
    }



    template <class ...Args>
    void outn(StrView fmts, Args&&... args)  {
        outn(fmt(fmts, std::forward<Args>(args)...));
    }




};

}  // namespace apl

