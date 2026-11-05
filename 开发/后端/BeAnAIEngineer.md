Here’s the order I’d follow:

1. **Python + basics**: solid Python, Git, SQL, APIs. Most “AI engineer” jobs are software engineering first.
2. **Light math**: just linear algebra, probability and gradients. Learn it as you go, don’t spend months on it up front.
3. **Classic ML**: scikit-learn, train/test splits, overfitting, metrics. Evaluation habits matter more than fancy models.
4. **Deep learning**: PyTorch basics, then transformers. The [HF LLM Course](https://huggingface.co/learn/llm-course) is free and covers Transformers, Tokenizers, fine-tuning and more.
5. **Build with LLMs**: RAG, tool calling, agents. The [HF Agents Course](https://huggingface.co/learn/agents-course) is a good hands-on start.
6. **Ship it**: deploy a few projects (API, Space, Docker) and add evaluation and monitoring.

**What the market wants:** from the job-description analyses I found, most AI engineer roles are LLM apps, RAG, agents, evaluation and production deployment, not model research.

**What to skip at first:** deep math proofs, reading lots of papers, training big models from scratch, and learning every framework (pick one stack and go deep).

**Rough emphasis:** math (light), ML (medium), deep learning (medium), generative AI (high), software engineering and deployment (high).

Best advice: build 2-3 real projects (e.g. a RAG app over your own documents, with evals) and publish them on the Hub/GitHub. That counts more than certificates.