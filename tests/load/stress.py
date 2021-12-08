from locust import HttpUser, task, between, tag


text_en = "Importantly, Rocket also aims to be fun, and it accomplishes this by ensuring that you write as little code as needed to accomplish your task."
text_es = "Es importante destacar que Rocket también tiene como objetivo ser divertido, y lo logra asegurándose de que escriba tan poco código como sea necesario para realizar su tarea."


class TranslatorUser(HttpUser):
    wait_time = between(3, 5)

    @tag('single')
    @tag('mixed')
    @task
    def translate_en_es(self):
        self.client.post("/v1/translate", json={"from":"en", "to":"es", "text": text_en})

    @tag('mixed')
    @task
    def translate_en_de(self):
        self.client.post("/v1/translate", json={"from":"en", "to":"de", "text": text_en})

    @tag('mixed')
    @task
    def translate_en_cs(self):
        self.client.post("/v1/translate", json={"from":"en", "to":"cs", "text": text_en})

    @tag('mixed')
    @task
    def translate_es_en(self):
        self.client.post("/v1/translate", json={"from":"es", "to":"en", "text": text_es})

    @tag('mixed')
    @task
    def translate_pivot_es_et(self):
        self.client.post("/v1/translate", json={"from":"es", "to":"et", "text": text_es})

    @tag('mixed')
    @task
    def translate_pivot_es_cs(self):
        self.client.post("/v1/translate", json={"from":"es", "to":"cs", "text": text_es})

    @tag('mixed')
    @tag('pivot')
    @task
    def translate_pivot_es_de(self):
        self.client.post("/v1/translate", json={"from":"es", "to":"de", "text": text_es})