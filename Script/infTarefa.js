function formatarGB(bytes) {
    return (bytes / (1024 * 1024 * 1024)).toFixed(2) + " GB";
}

async function carregarDadosSistema() {
    try {
        const resposta = await fetch("../json/sistema.json");
        const dados = await resposta.json();

        // CPU
        document.getElementById("cpuValor").textContent =
            dados.cpu_uso.toFixed(1) + "%";
        document.getElementById("cpuBarra").style.width =
            dados.cpu_uso + "%";

        // RAM
        const ramUsada = formatarGB(dados.ram_usada);
        const ramTotal = formatarGB(dados.ram_total);
        document.getElementById("ramValor").textContent =
            `${ramUsada} / ${ramTotal}`;

        const ramPercent = (dados.ram_usada / dados.ram_total) * 100;
        document.getElementById("ramBarra").style.width =
            ramPercent + "%";

        // Disco
        const discoUsado = formatarGB(dados.disco_usado);
        const discoTotal = formatarGB(dados.disco_total);
        document.getElementById("discoValor").textContent =
            `${discoUsado} / ${discoTotal}`;

        const discoPercent = (dados.disco_usado / dados.disco_total) * 100;
        document.getElementById("discoBarra").style.width =
            discoPercent + "%";

        // Processos
        document.getElementById("procValor").textContent =
            dados.processos;

        // Threads
        document.getElementById("threadsValor").textContent =
            dados.threads;

        // Timestamp
        document.getElementById("timestampValor").textContent =
            dados.timestamp;

    } catch (erro) {
        console.error("Erro ao carregar sistema.json:", erro);
    }
}

// atualiza a cada 2 segundos
setInterval(carregarDadosSistema, 2000);

// primeira leitura ao abrir a página
carregarDadosSistema();