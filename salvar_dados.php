<?php
// --- DADOS DE CONEXÃO ---
$servername = "localhost";
$username = "root";      // Usuário padrão do XAMPP
$password = "";          // Senha padrão do XAMPP (vazia)
$dbname = "projeto_sesi";

// Cria a conexão com o banco de dados MySQL
$conn = new mysqli($servername, $username, $password, $dbname);

// Verifica se houve falha na conexão
if ($conn->connect_error) {
    die("Falha na conexão com o banco de dados: " . $conn->connect_error);
}

// --- RECEBE E VALIDA OS DADOS DA ESP32 ---
if (isset($_POST['velocidade']) && isset($_POST['rpm'])) {
    
    // Captura os dados e converte para os tipos corretos
    $vel = floatval($_POST['velocidade']);
    $rpm = intval($_POST['rpm']);
    
    // Insere os dados na tabela do banco
    $sql = "INSERT INTO telemetria (velocidade, rpm) VALUES ($vel, $rpm)";
    
    if ($conn->query($sql) === TRUE) {
        echo "Dados salvos com sucesso!";
    } else {
        echo "Erro ao salvar no banco: " . $conn->error;
    }
} else {
    echo "Erro: Requisição inválida. Dados insuficientes.";
}

// Fecha a conexão
$conn->close();
?>