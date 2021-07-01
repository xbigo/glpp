#ifndef GLPP_H
#define GLPP_H
#pragma once
#include <span>
#include <memory>
#include <string>
#include <string_view>
#include <optional>
#include <functional>
#include <cassert>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace glpp {
    template <class Elem, class Traits = std::char_traits<Elem>>
    class basic_ndstring_view : public std::basic_string_view<Elem, Traits > { // wrapper for any kind of contiguous character buffer
    public:
        using base_type = std::basic_string_view<Elem, Traits >;
        using typename base_type::const_pointer;
        using typename base_type::size_type;


        constexpr basic_ndstring_view() noexcept = default;

        constexpr basic_ndstring_view(const basic_ndstring_view&) noexcept = default;
        constexpr basic_ndstring_view& operator=(const basic_ndstring_view&) noexcept = default;

        /* implicit */ constexpr basic_ndstring_view(const_pointer str) noexcept // strengthened
            : base_type(str){}

        // \pre str[m_size] is readable
        constexpr basic_ndstring_view(const_pointer str, size_type count) noexcept // strengthened
            : base_type(str, count) {
            is_null_terminated(); //just check readable
        }

        constexpr void swap(basic_ndstring_view& other_) noexcept {
            base_type::swap(other_);
        }

        [[nodiscard]] constexpr const_pointer c_str() const noexcept {
            assert(is_null_terminated());
            return this->data();
        }
        [[nodiscard]] constexpr bool is_null_terminated() const noexcept {
            return this->data() && this->data()[this->size()] == Elem{};
        }
    };

    template <class Elem, class Traits = std::char_traits<Elem>>
    class basic_ntstring_view : public basic_ndstring_view<Elem, Traits>
    {
    public:
        using base_type = basic_ndstring_view<Elem, Traits >;
        using typename base_type::const_pointer;
        using typename base_type::size_type;

        constexpr basic_ntstring_view() noexcept = default;

        constexpr basic_ntstring_view(const basic_ntstring_view&) noexcept = default;
        constexpr basic_ntstring_view& operator=(const basic_ntstring_view&) noexcept = default;

        /* implicit */ constexpr basic_ntstring_view(const_pointer str) noexcept // strengthened
            : base_type(str) {}

        // \pre str[m_size] is readable
        constexpr basic_ntstring_view(const_pointer str, size_type count) noexcept // strengthened
            : base_type(str, count) {
            assert(this->is_null_terminated()); //check null terminater
        }

        constexpr void swap(basic_ntstring_view& other_) noexcept {
            base_type::swap(other_);
        }

        [[nodiscard]] constexpr const_pointer c_str() const noexcept {
            return this->data();
        }
    };

    using ndstr_view = basic_ndstring_view<char>;  // null terminater detectable string_view
    using ndwstr_view = basic_ndstring_view<wchar_t>;  // null terminater detectable string_view
    using ntstr_view = basic_ntstring_view<char>;  // null terminated string_view
    using ntwstr_view = basic_ntstring_view<wchar_t>;  // null terminated string_view

    template <class Elem, class Traits>
    inline constexpr basic_ntstring_view<Elem, Traits> to_view(const std::basic_string<Elem, Traits>& str) noexcept {
        return basic_ntstring_view<Elem, Traits>(str.c_str(), str.size());
    }

    struct rect_scale_t {
        int width = 0;
        int height = 0;
    };
    class Monitor;
    class Window;
    class GlobalContext;
    class ProgramBuilder;

    class Window {
        GLFWwindow* m_win = nullptr;
        explicit Window(GLFWwindow* p) : m_win(p) {}

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
    public:
        Window() noexcept= default;
        ~Window() noexcept {
            if (m_win) glfwDestroyWindow(m_win);
        }
        void swap(Window& rhs) noexcept {
            std::swap(m_win, rhs.m_win);
        }
        Window(Window&& rhs) noexcept
            : m_win(rhs.m_win) {
            rhs.m_win = nullptr;
        }
        Window& operator=(Window&& rhs) noexcept {
            Window(std::move(rhs)).swap(*this);
            return *this;
        }

        void setShouldClose(int v);
        int getShouldClose() const;
        void swapBuffer();
        int getKey(int key);

        static Window make(rect_scale_t rect, ntstr_view title, std::optional<std::reference_wrapper<Monitor>> monitor = {}, std::optional<std::reference_wrapper<Window>> share = {});
    };

    class Monitor {
        GLFWmonitor* m_monitor;
        friend class Window;
    };

    template <class Range>
    concept string_view_range = std::ranges::range<std::remove_cvref_t<Range>>
        && std::is_convertible_v<std::remove_reference_t<decltype(*std::cbegin(std::declval<std::remove_cvref_t<Range>>()))>, std::string_view>;

    class Shader {
        friend class ProgramBuilder;
        GLuint m_id = 0;
        explicit Shader(GLuint id) : m_id(id) {}
        static Shader make(GLuint  type, GLsizei  n, const char* const* sources, const GLint* lengths);

    public:
        ~Shader();
        Shader() = delete;
        Shader(Shader&& rhs) : m_id(rhs.m_id) {
            rhs.m_id = 0;
        }
        Shader& operator=(Shader&& rhs) {
            Shader(std::move(rhs)).swap(*this); 
            return *this;
        }
        void swap(Shader& rhs) {
            std::swap(m_id, rhs.m_id);
        }

        template<string_view_range Range>
        static Shader make(GLuint type, Range&& text) {
            std::vector<const char*> sources;
            std::vector< GLint> lengths;
            for (auto& i : text) {
                std::string_view s(i);
                lengths.emplace_back(GLint(s.size()));
                sources.emplace_back(s);
            }
            return make(type, GLsizei(sources.size()), sources.data(), lengths.data());
        }
        static Shader make(GLuint type, std::string_view text) {
            GLint size(static_cast<GLint>(text.size()));
            const char* str = text.data();
            return make(type, 1, &str, &size);
        }

        static Shader makeVertex(std::string_view text) {
            return make(GL_VERTEX_SHADER, text);
        }
        template<string_view_range Range>
        static Shader makeVertex(Range&& text) {
            return make(GL_VERTEX_SHADER, std::forward<Range>(text));
        }
        static Shader makeFragment(std::string_view text) {
            return make(GL_FRAGMENT_SHADER, text);
        }
        template<string_view_range Range>
        static Shader makeFragment(Range&& text) {
            return make(GL_FRAGMENT_SHADER, std::forward<Range>(text));
        }
    };

    
    class Program
    {
        friend class ProgramBuilder;
        GLuint m_id = 0;
        Program(GLuint id) : m_id(id) {};
    public:
        Program() = default;
        ~Program();
        Program(Program&& rhs) : m_id(rhs.m_id) {
            rhs.m_id = 0;
        }
        Program& operator=(Program&& rhs) {
            Program(std::move(rhs)).swap(*this);
            return *this;
        }
        void swap(Program& rhs) {
            std::swap(m_id, rhs.m_id);
        }
        bool valid() const {
            return m_id != 0;
        }
        void use();

        int index_of_uniform(ntstr_view param);
    };

    class ProgramBuilder
    {
        GLuint m_id = 0;
    public:
        ProgramBuilder();
        ProgramBuilder(ProgramBuilder&& rhs) : m_id(rhs.m_id) {
            rhs.m_id = 0;
        }
        ProgramBuilder& operator=(ProgramBuilder&& rhs) {
            ProgramBuilder(std::move(rhs)).swap(*this);
            return *this;
        }
        void swap(ProgramBuilder& rhs) {
            std::swap(m_id, rhs.m_id);
        }
        bool valid() const {
            return m_id != 0;
        }
        ProgramBuilder& renew();
        // \pre valid()
        ProgramBuilder& addShader(const Shader& rhs);

        // \pre valid()
        // \post !valid()
        Program build();
    };

    class GlobalContext {
        GlobalContext(const GlobalContext&) = delete;
        GlobalContext& operator=(GlobalContext&) = delete;

        GlobalContext();
    public:
        ~GlobalContext();
        static GlobalContext init();
    };

    using data_ptr = std::unique_ptr<std::byte, void(*)(void*)>;

    class Image {
        int m_width = 0;
        int m_height = 0;
        int m_nrChannels = 0;
        data_ptr m_data;
    public:

        Image() = default;
        Image(Image&& rhs) 
            : m_width(rhs.m_width)
            , m_height(rhs.m_height)
            , m_nrChannels(rhs.m_nrChannels)
            , m_data(std::move(rhs.m_data))
        {
            rhs.m_width = rhs.m_height = rhs.m_nrChannels = 0;
        }
        Image(int w, int h, int ch, data_ptr&& data) 
            : m_width(w)
            , m_height(h)
            , m_nrChannels(ch)
            , m_data(std::move(data)) {}
        Image& operator==(Image&& rhs) {
            Image(std::move(rhs)).swap(*this);
        }

        void swap(Image& rhs) {
            using std::swap;
            swap(m_width, rhs.m_width);
            swap(m_height, rhs.m_height);
            swap(m_nrChannels, rhs.m_nrChannels);
            m_data.swap(rhs.m_data);
        }
        const std::byte* raw_data() const {
            return m_data.get();
        }
        //std::span<const std::byte> data() const {
        //    return m_data ? {} : {};
        //}

        int width() const { return m_width; }
        int height() const { return m_height; }
        int channels() const { return m_nrChannels; }
    };

    Image loadImage(ntstr_view path);
}

#endif //GLPP_H