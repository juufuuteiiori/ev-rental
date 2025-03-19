from fastapi import FastAPI
from transformers import pipeline
from pydantic import BaseModel

app = FastAPI()

class TextRequest(BaseModel):
    text: str

# 指定本地模型路径
sentiment_model_path = "models/multilingual-sentiment-analysis"
toxicity_model_path = "models/xlmr-large-toxicity"

# 加载 pipeline
sentiment_pipe = pipeline("text-classification", model=sentiment_model_path, tokenizer=sentiment_model_path)
toxicity_pipe = pipeline("text-classification", model=toxicity_model_path, tokenizer=toxicity_model_path)

@app.post("/sentiment")
async def analyze_sentiment(request: TextRequest):
    """ 处理情感分析请求 """
    result = sentiment_pipe(request.text)
    return result

@app.post("/toxicity")
async def analyze_toxicity(request: TextRequest):
    """ 处理有害言论检测请求 """
    result = toxicity_pipe(request.text)
    return result

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8002, log_level="debug")