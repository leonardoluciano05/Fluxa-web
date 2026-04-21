function mostrar(tela, elemento) {

    // troca a classe do html
    document.documentElement.className = tela;

    // remove active de todos
    const itens = document.querySelectorAll(".menu li");
    itens.forEach(item => item.classList.remove("active"));

    // adiciona no clicado
    if (elemento) {
        elemento.classList.add("active");
    }

    // salva
    localStorage.setItem("telaAtual", tela);
}

// inicialização
document.addEventListener("DOMContentLoaded", () => {
    const telaSalva = localStorage.getItem("telaAtual") || "home";

    document.documentElement.className = telaSalva;

    const itemMenu = document.querySelector(`[onclick*="${telaSalva}"]`);
    if (itemMenu) {
        itemMenu.classList.add("active");
    }
});